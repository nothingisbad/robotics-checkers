#ifndef CHECKERS_TRACK_HPP
#define CHECKERS_TRACK_HPP

/**************************************************************************
 *  Copyright 2008,2009 KISS Institute for Practical Robotics             *
 *                                                                        *
 *  This file is part of CBC Firmware.                                    *
 *                                                                        *
 *  CBC Firmware is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  CBC Firmware is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this copy of CBC Firmware.  Check the LICENSE file         *
 *  in the project root.  If not, see <http://www.gnu.org/licenses/>.     *
 **************************************************************************/
/* modified to hopefully get the hsv model garbage working. */

#include "./SharedMem.hpp"

typedef struct BlobResultsStr {
  // area, in pixels
  int area;
  // centroid location
  float x;
  float y;
  // confidence, 0-100
  float confidence;
  int bbox_left, bbox_top, bbox_right, bbox_bottom;
  
  // angle in radians of the major axis of the blob.
  // Zero is horizontal and when the left end is higher than the right end the angle will be positive.
  // The range is -PI/2 to +PI/2.
  float angle;
  // sizes, in pixels, of major and minor axes
  float major_axis;
  float minor_axis;
} BlobResults;

#define CHANNEL_MAX_BLOBS 10
typedef struct ChannelResultsStr {
  int n_blobs;
  int hsv_model[4];
  BlobResults blobs[CHANNEL_MAX_BLOBS];
} ChannelResults;

#define TRACKING_MAX_CHANNELS 4
typedef struct TrackingResultsStr {
  int frame_number;
  int frame_time;
  int previous_frame_time;
  int n_channels;
  ChannelResults channels[TRACKING_MAX_CHANNELS];
} TrackingResults;


bool tracklib_initted = false;

/* well, it works in their GUI, so I hope it works here. */
SharedMem<TrackingResults> *tracklib_sm_results;
TrackingResults tracklib_results_snapshot;

void track_init() {
  if (!tracklib_initted) {
    tracklib_initted = true;
    tracklib_sm_results = new SharedMem<TrackingResults>("/tmp/color_tracking_results");
  }
}

int track_is_new_data_available() {
  track_init();
  return ((TrackingResults*)shared_mem_ptr(tracklib_sm_results))->frame_number != tracklib_results_snapshot.frame_number;
}

void track_update()
{
  track_init();
  tracklib_sm_results->read( tracklib_results_snapshot );
  //shared_mem_read(tracklib_sm_results, &, sizeof(tracklib_results_snapshot));
}

int track_get_frame() {
  track_init();
  return tracklib_results_snapshot.frame_number;
}

int track_capture_time() {
  track_init();
  return tracklib_results_snapshot.frame_time;
}

int track_previous_capture_time() {
  track_init();
  return tracklib_results_snapshot.previous_frame_time;
}

static int channel_in_bounds(int ch) {
  return 0 <= ch && ch < tracklib_results_snapshot.n_channels;
}

static int blob_in_bounds(int ch, int i) {
  if (!channel_in_bounds(ch)) return 0;
  return 0 <= i && i < tracklib_results_snapshot.channels[ch].n_blobs;
}

int track_count(int ch) {
  if (!channel_in_bounds(ch)) return -1;
  return tracklib_results_snapshot.channels[ch].n_blobs;
}

int track_size(int ch, int i) {
  if (!blob_in_bounds(ch, i)) return -1;
  return tracklib_results_snapshot.channels[ch].blobs[i].area;
}

int track_x(int ch, int i) {
  if (!blob_in_bounds(ch, i)) return -1;
  return (int)(tracklib_results_snapshot.channels[ch].blobs[i].x);
}

int track_y(int ch, int i) {
  if (!blob_in_bounds(ch, i)) return -1;
  return (int)(tracklib_results_snapshot.channels[ch].blobs[i].y);
}

// Gets the confidence for seeing the blob as a percentage of the blob pixel area/bounding box area (range 0-100, low numbers bad, high numbers good)
int track_confidence(int ch, int i) {
  if (!blob_in_bounds(ch, i)) return -1;
  return (int)(tracklib_results_snapshot.channels[ch].blobs[i].confidence);
}


// Gets the pixel x coordinate of the leftmost pixel in the blob
int track_bbox_left(int ch, int i) {
  if (!blob_in_bounds(ch, i)) return -1;
  return tracklib_results_snapshot.channels[ch].blobs[i].bbox_left;
}

// Gets the pixel x coordinate of the rightmost pixel in the blob
int track_bbox_right(int ch, int i) {
  if (!blob_in_bounds(ch, i)) return -1;
  return tracklib_results_snapshot.channels[ch].blobs[i].bbox_right;
}

// Gets the pixel y coordinate of the topmost pixel in the blob
int track_bbox_top(int ch, int i) {
  if (!blob_in_bounds(ch, i)) return -1;
  return tracklib_results_snapshot.channels[ch].blobs[i].bbox_top;
}

// Gets the pixel y coordinate of the bottommost pixel in the blob
int track_bbox_bottom(int ch, int i) {
  if (!blob_in_bounds(ch, i)) return -1;
  return tracklib_results_snapshot.channels[ch].blobs[i].bbox_bottom;
}

// Gets the pixel x width of the bounding box of the blob. This is equivalent to track_bbox_right - track_bbox_left + 1
int track_bbox_width(int ch, int i) {
  if (!blob_in_bounds(ch, i)) return -1;
  return track_bbox_right(ch, i) - track_bbox_left(ch, i) + 1;
}

// Gets the pixel y height of the bounding box of the blob. This is equivalent to track_bbox_bottom - track_bbox_top + 1
int track_bbox_height(int ch, int i) {
  if (!blob_in_bounds(ch, i)) return -1;
  return track_bbox_bottom(ch, i) - track_bbox_top(ch, i) + 1;
}

// Gets the angle in radians of the major axis of the blob.
// Zero is horizontal and when the left end is higher than the right end the angle will be positive.
// The range is -PI/2 to +PI/2.
float track_angle(int ch, int i) {
  if (!blob_in_bounds(ch, i)) return -1;
  return tracklib_results_snapshot.channels[ch].blobs[i].angle;
}

// Gets the length in pixels of the major and minor axes of the bounding ellipse
int track_major_axis(int ch, int i) {
  if (!blob_in_bounds(ch, i)) return -1;
  return (int)(tracklib_results_snapshot.channels[ch].blobs[i].major_axis);
}

int track_minor_axis(int ch, int i) {
  if (!blob_in_bounds(ch, i)) return -1;
  return (int)(tracklib_results_snapshot.channels[ch].blobs[i].minor_axis);
}

void track_set_model_hsv(int ch, int h_min, int h_max, int s_min, int v_min) {
    if (!channel_in_bounds(ch)) return;
    tracklib_results_snapshot.channels[ch].hsv_model[0] = h_min;
    tracklib_results_snapshot.channels[ch].hsv_model[1] = h_max;
    tracklib_results_snapshot.channels[ch].hsv_model[2] = s_min;
    tracklib_results_snapshot.channels[ch].hsv_model[3] = v_min;

    tracklib_sm_results.write(tracklib_results_snapshot);
}

void track_get_model_hsv(int ch, int *h_min, int *h_max, int *s_min, int *v_min)
{
    if (!channel_in_bounds(ch)) return;
    *h_min = tracklib_results_snapshot.channels[ch].hsv_model[0];
    *h_max = tracklib_results_snapshot.channels[ch].hsv_model[1];
    *s_min = tracklib_results_snapshot.channels[ch].hsv_model[2];
    *v_min = tracklib_results_snapshot.channels[ch].hsv_model[3];
}

#endif
