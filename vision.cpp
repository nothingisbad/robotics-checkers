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
#include "./Track.hpp"

#include <nnet/utility.hpp>

#include <cbc.h>

using namespace std;

static const int size_threshold = 25;

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
iPair grid_position(const fPair& pixel, const Transform& transform) {
  int col, row;
    
  col = transform[0][0] * pixel.x  + transform[0][1] * pixel.y;
  row = transform[1][0] * pixel.x  + transform[1][1] * pixel.y + 0.5;
    
  return iPair( -row, col );
}

void capture_color(Grid &g, const fPair& origin, int channel, const Transform& transform) {
  iPair tmp;
  for(int i = 0; (i < 10) && track_size(channel,i) > size_threshold; ++i) {
    tmp = grid_position( fPair( capture_point(channel, i) ) - origin, transform);
    cout << "looking at " << tmp << " x: " << setw(4) << track_x(channel,i) - origin.x   << endl;
    if(track_size(channel,i) < 90
       && (tmp >= iPair(0,0))
       && (tmp < iPair(8,8)) ) {
      --g.at( tmp );
    }}}

int main() {
  cout << "Begin" << endl;
  Grid g{};
  Transform transform;
  fPair origin, edge;
  int frame_number
    , frame_attempts = 0;

  array<iPair,4> corner_marker{};
  track_update();

  for(int i = 0; i < 4; ++i) {

    /* aquire corner_markers */
    for(int i = 0; i < 4; ++i) 
      corner_marker[i] = capture_point(2,i);

    sort_edges(corner_marker);

    /* test for skew */
    if( corner_marker[1].x >= corner_marker[2].y + 8
	&&  corner_marker[1].x <= corner_marker[2].y - 8)
      {
	cout << "(skewed; x length: " << corner_marker[1].x
	     << "y length: " << corner_marker[2].y << ")" << endl;}

    /* average the length of the four edges */
    /* I'm taking the average, and under rotation a +y shift on the X axis would corrospond to a
       -x shift on the Y axis */
    fPair tmp = (corner_marker[0] - corner_marker[2]  + corner_marker[1] - corner_marker[3]).transpose();
    tmp.y = -tmp.y;

    edge = (corner_marker[1] - corner_marker[0] + corner_marker[3] - corner_marker[2])
      + tmp;

    edge = edge / 4;

    float c, s;

    // cout << "edge: " << edge << " corner[1] - corner[0]: " << corner_marker[1] - corner_marker[0] <<  endl;
    // cout << "(corner[0] - corner[2]  + corner[1] - corner[3]).transpose();" << (corner_marker[0] - corner_marker[2]  + corner_marker[1] - corner_marker[3]).transpose() << endl;
    if( abs(edge.x) > 2 && abs(edge.y) > 2 ) {
      cout << "Rotated" << endl;
      float hypotenuse_squared = edge.x * edge.x + edge.y * edge.y;

      c = (9.0f * edge.x) / hypotenuse_squared;
      s = (9.0f * edge.y) / hypotenuse_squared;
    } else {
      c = 9.0f / edge.x;
      s = 0;
    }

    transform = {{ array<float,2>{{c, s}}
		   , array<float,2>{{-s, c}}}};

    /* compensate for origin offset (markers aren't right on the corner)  */
    origin = fPair(corner_marker[0]) + (edge / 18);

    capture_color(g, origin, 1, transform);
    //capture_color(g, corner_marker[0], 2, transform);


    do {
      sleep(0.25);
      track_update();
      ++frame_attempts;
      if(frame_attempts > 20) {
	cout << "Fucking camera froze." << endl;
	exit(1);
      }
    } while( frame_number == track_get_frame() );
    frame_number = track_get_frame();
    frame_attempts = 0;
  }

  //cout << "origin " << origin << " edge " << edge << endl;
  cout << "transform\n";
  print_array(transform[0]) << endl;
  print_array(transform[1]) << endl;
  g.squelch(2);
  cout << g << flush;

  return 0;
}