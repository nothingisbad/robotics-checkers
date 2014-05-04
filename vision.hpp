#ifndef VISION_HPP
#define VISION_HPP
/**
 * Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Kaitlyn Carcia <kate.carcia@gmail.com>
 * @file /home/ryan/uml/robotics/checkers/vision.hpp
 * Created on Apr 30, 2014
 */


/* Includes */
#include <vector>
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

static const int size_threshold = 25 /* blob size threshold */
  , marker_channel = 2 /* green */
  , human_channel_A = 1 /* red */
  , human_channel_B = 0; /* yellow */

typedef array<array<float,2>,2> Transform;

void sort_edges(array<iPair,4>& marker) {
  /* y has the lowest y-coordinate (top left), stick it
     at the end of the list */
  sort(marker.begin(), marker.end()
       , [](const iPair &a, const iPair &b) {
	 return a.y > b.y;
       });

  /* Sorting green blobs - if marker[0] has a greater X value,
   * it's not the origin so swap it around */
  if(marker[0].x > marker[1].x) swap(marker[0], marker[1]);
  if(marker[2].x > marker[3].x) swap(marker[2], marker[3]);
}

/* Capturre x and y pixel given channel and index */
iPair capture_point(int channel, int index) {
  return iPair(track_x(channel, index)
	       , track_y(channel, index));
}

/* Calculate grid position */
iPair grid_position(const fPair& pixel, const Transform& transform) {
  int col, row;
    
  col = transform[0][0] * pixel.x  + transform[0][1] * pixel.y;
  row = transform[1][0] * pixel.x  + transform[1][1] * pixel.y + 0.5;
    
  return iPair( -row, col );
}

/* Place coordinate on grid */
void capture_color(Grid &g, const fPair& origin, int channel, const Transform& transform) {
  iPair tmp;
  for(int i = 0; (i < 10) && track_size(channel,i) > size_threshold; ++i) {
    tmp = grid_position( fPair( capture_point(channel, i) ) - origin, transform);

    if(track_size(channel,i) < 90
       && (tmp >= iPair(0,0))
       && (tmp < iPair(8,8)) ) {
      --g.at( tmp );
    }}}

Grid capture_grid() {
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
      corner_marker[i] = capture_point(marker_channel,i);

    /* Sort corner markers to determine position of axes */
    sort_edges(corner_marker);

    /* test for skew */
    if( corner_marker[1].x >= corner_marker[2].y + 8
	&&  corner_marker[1].x <= corner_marker[2].y - 8)
      {
	cout << "(skewed; x length: " << corner_marker[1].x
	     << "y length: " << corner_marker[2].y << ")" << endl;}

    /* Average the length of the four edges */
    /* I'm taking the average, and under rotation a +y shift on the X axis would corrospond to a
       -x shift on the Y axis */
    fPair tmp = (corner_marker[0] - corner_marker[2]  + corner_marker[1] - corner_marker[3]).transpose();
    tmp.y = -tmp.y;

    edge = (corner_marker[1] - corner_marker[0] + corner_marker[3] - corner_marker[2])
      + tmp;

    edge = edge / 4;

    float c, s;

    if( abs(edge.x) > 2 && abs(edge.y) > 2 ) {
      float hypotenuse_squared = edge.x * edge.x + edge.y * edge.y;

      /* I can get the position of one blob relative to the origin into the rotated frame with the translation matrix,
	   * then I divide that by edge-length/9 (since the edge markers are each 1/2 square away from the actual board)
	   * to get the position of the square it's occupying.  If I combine the two steps I don't have to take a square root
	   * to compute the trig functions */
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

    /* Capture human pieces */
    capture_color(g, origin, human_channel_A, transform);
    capture_color(g, origin, human_channel_B, transform);

    /* Attemps to capture board */
    do {
      sleep(0.25);
      track_update();
      ++frame_attempts;
      if(frame_attempts > 20) {
        cout << "Camera froze." << endl;
        exit(1);
      }
    } while( frame_number == track_get_frame() );
    frame_number = track_get_frame();
    frame_attempts = 0;
  }
  g.squelch(2);
  return g;
}

#endif
