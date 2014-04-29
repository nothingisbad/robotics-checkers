// Kaitlyn Carcia
// Last Updated April 22, 2014

#include <vector>
#include <track.h>
#include <iostream>
#include <algorithm>
#include <tuple>

#include <compat.h>

#include "./Grid.hpp"
#include "./points.hpp"
#include "./hsv.hpp"

#include <cbc.h>

using namespace std;

typedef array<array<float,2>,2> Transform;

void sort_edges(array<iPair,4>& marker) {
  /* y has the lowest y-coordinate (top left), stick it
     at the end of the list */
  sort(marker.begin(), marker.end()
       , [](const iPair &a, const iPair &b) {
	 return a.y > b.y;
       });

  /* if marker[0] has a greater X value, it's not the origin
     so swap it around */
  if(marker[0].x > marker[1].x) swap(marker[0], marker[1]);
  if(marker[2].x > marker[3].x) swap(marker[2], marker[3]);
}

iPair capture_point(int channel, int index) {
  return iPair(track_x(channel, index)
	       , track_y(channel, index));
}

/* damn.  If only we had monads. */
iPair grid_position(const iPair& pixel, const Transform& transform) {
  int col, row;
    
  col = transform[0][0] * pixel.x  + transform[0][1] * pixel.y;
  row = transform[1][0] * pixel.x  + transform[1][1] * pixel.y;
    
  return iPair( -row , col );
}

void capture_color(Grid &g, const iPair& origin, int channel, const Transform& transform) {
  iPair tmp;
  for(int i = 0; (i < 10) && track_size(channel,i) > 40; ++i) {
    tmp = grid_position(capture_point(channel, i) - origin, transform);
    /* cout << "looking at " << tmp << " size " << track_size(channel,i) << endl; */
    if(track_size(channel,i) < 90
       && (tmp > iPair(0,0))
       && (tmp < iPair(8,8)) ) {
      --g.at( tmp );
    }}}

int main() {
  cout << "Begin" << endl;
  Grid g;

  int i;
  array<iPair,4> corner_marker;

  adjust_hsv(1, 4, 30, 70);
  adjust_hsv(2, 2, 30, 70);

  /* aquire corner_markers */
 retry_markers:
  track_update();		
		
  for(i = 0; i < 4; ++i)
    corner_marker[i] = capture_point(1,i);

  sort_edges(corner_marker);

  /* test for skew */
  if( corner_marker[1].x >= corner_marker[2].y + 8
      &&  corner_marker[1].x <= corner_marker[2].y - 8)
    {
      cout << "(skewed; x length: " << corner_marker[1].x
	   << "y length: " << corner_marker[2].y << ")" << endl;

      goto retry_markers;
    }

  cout << "have edges" << endl;
  iPair edge = corner_marker[1] - corner_marker[0];
  float c, s
    , hypotenuse_squared = edge.x * edge.x + edge.y * edge.y;

  cout << "edge: " << edge << endl;

  if( edge.x > 0.01 && edge.y > 0.01 ) {
    cout << "Rotation deteced" << endl;
    c = (8.0f * corner_marker[1].x) / hypotenuse_squared;
    s = (8.0f * corner_marker[1].y) / hypotenuse_squared;
  } else {
    c = 8.0f / edge.x;
    s = 0;
  }

  Transform transform = {{ array<float,2>{{c, -s}}
			   , array<float,2>{{s, c}}}};

  cout << "Transform setup done; press A to continue" << endl;
  while( !a_button() ) sleep(0.1);

  for(int j = 0; j < 4; ++j) {
    capture_color(g, corner_marker[0], 1, transform);
    capture_color(g, corner_marker[0], 2, transform);

    track_update(); 
  }

  g.squelch(2);
  cout << g << endl;

  return 0;
}
