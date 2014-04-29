#ifndef CHECKERS_HSV_HPP
#define CHECKERS_HSV_HPP
/**
 * @file /home/ryan/uml/robotics/checkers/hsv.hpp
 * @author Ryan Domigan <ryan_domigan@sutdents@uml.edu>
 * Created on Apr 25, 2014
 *
 * Adding some HSV auto-calibration to see if that works any better.
 * This functionality is probably buried in the cbc source tree somewhere,
 * but I can't find it yet.  And I mean really.  It bugs the crap
 * out of me that they can capture N elements on their tracking screen then
 * They fucking only pass 10 on to the API.
 */

#include <iostream>
#include <array>
#include <algorithm>
#include <chrono>

#include "./Track.h"



using namespace std;

void adjust_hsv(int channel, int expected_count, int min_size, int max_size) {
  static const int initial_step_size = 10;
  int varying = 0
    , step_size = initial_step_size
    , last_expected = expected_count - 1

    , hue_center;
  
  array<int, 3> hsv;

  auto size = [&](int i) { return track_size(channel, i); };
  auto count = [&]() { return track_count(channel); };

  auto is_too_narrow = [&]() -> bool {
    return  expected_count > count()
      || min_size > size(last_expected);
  };

  auto is_too_wide = [&]() -> bool {
    return size(0) > max_size
      && size(expected_count) > size(0) / 2;
  };

  auto get_hsv = [&]() {
    int min_hue, max_hue;

    track_get_model_hsv(channel, &min_hue, &max_hue, &hsv[1], &hsv[2]);
    cout << "Track get model:" << min_hue << " : " << max_hue << " : " << hsv[1] << " : " << hsv[2] << endl;

    hue_center = (((long)min_hue) + ((long)max_hue)) / 2;
    hsv[0] = (max_hue - min_hue) / 2;

    /* these are mins, and I want adding to make them wider */
    hsv[1] = -hsv[1];
    hsv[2] = -hsv[2];
  };

  auto set_hsv = [&]() {
    track_set_model_hsv(channel
			, hue_center - hsv[0]
			, hue_center + hsv[0]
			, -hsv[1], -hsv[2]); };

  auto grow_blob = [&](int a, int b) { return a > b; };
  auto shrink_blob = [&](int a, int b) { return a < b; };

  auto tweak_hue = [&](const std::function<bool(int,int)>& comp) {
    int  initial = size( 0 );

    hue_center += step_size;
    set_hsv();
    track_update();
    if( comp(size(0), initial) ) return;
    hue_center -= 2 * step_size;
    track_update();
    if( comp(size(0), initial) ) return;
    cout << "tweak_hue : Seems to be a critical point at size " << initial << endl;
  };

  auto next_parameter = [&]() {
    ++varying;
    if(varying % 5 == 0) {
      step_size = max(1, step_size >> 1); /* we've overshot on every parameter, reduce the step size */
      varying = 0;
    }};

  auto wider_hsv = [&]() {
    if(varying == 5) tweak_hue( grow_blob );
    else {
      hsv[varying] += step_size;
      track_update();
    }
  };

  auto narrow_hsv = [&]() {
    if(varying == 5) tweak_hue(shrink_blob);
    else {
      hsv[varying] -= step_size;
      track_update();
    }
  };

  /***************************/
  /*  __  __            _    */
  /* |  \/  | ___  __ _| |_  */
  /* | |\/| |/ _ \/ _` | __| */
  /* | |  | |  __/ (_| | |_  */
  /* |_|  |_|\___|\__,_|\__| */
  /***************************/
  enum { narrow = 1, wider = 2 };
  int last_action = 0;

  track_update();
  sleep(0.1);
  get_hsv();

  cout << "Initial HSV:" << hue_center << " : " << hsv[1] << " : " << hsv[2] << endl;
    
  for(int fail_count = 0; fail_count < 150; ++fail_count) {
    if( is_too_narrow() ) {
      if(last_action == wider)
	next_parameter();

      wider_hsv();
      last_action = wider;
      continue;
    }

    if( is_too_wide() ) {
      if(last_action == wider)
	next_parameter();

      narrow_hsv();
      last_action = narrow;
      continue;
    }

    std::cout << "Calibrate channel " << channel << " hue center: " << hue_center << endl;
    cout << "  final HSV:" << hue_center << " : " << hsv[1] << " : " << hsv[2] << endl;
    return;
  }

  std::cout << "Could not calibrate channel " << channel
	    << ", expected " << expected_count << " with "<< track_count(channel)
	    << " blobs found, largest " << track_size(channel, 0) << " and smallest "
	    << track_size(channel, min(track_count(channel)
				       , expected_count - 1));
  if(track_count(channel) >= expected_count)
    cout << " next smallest blob sized " << track_size(channel, expected_count) << endl;
  else cout << endl;
  exit(0);
}

#endif
