// Kaitlyn Carcia
// Last Updated April 9, 2014

#include <stdio.h>
#include <string.h>
#include <iostream>

#include <track.h>

#include "./Grid.hpp"

/* testing */
/* void track_update() {} */

/* /\* O: (~30, ~100) *\/ */
/* /\* X: (~130, ~100) *\/ */
/* /\* Y: (~30,  ~10) *\/ */
/* static int dummy_array[2][4][2] */
/* = {{{30, 100}			/\* channel 0 (the green markers) *\/ */
/*     , {130,100} */
/*     , {30,10} */
/*     , {389,11}}			/\* padding *\/ */

/*    , {{31, 14}      /\* 0,0 *\/ */
/*       , {20,10}   /\* 2,2 ish *\/ */
/*       , {45, 20} */
/*       , {45, 45}} */
/* }; */

/* int track_x(int channel, int n) { return dummy_array[channel][n][0]; } */

/* int track_y(int channel, int n) { return dummy_array[channel][n][1]; } */

// @x_origin	pixel x coordinate of origin
// @x_right		pixel x coordinate of rightmost part of the x axis
// Returns x length of every individual square
int square_length(int begin, int end) {
  return (end - begin) / 8;
}

#define MIN_IDX(A, dimention, a, b) ((A[(a)][dimention]) < (A[(b)][dimention]) ? (a) : (b))
#define MAX_IDX(A, dimention, a, b) ((A[(a)][(dimention)]) > (A[(b)][dimention]) ? (a) : (b))

void sort_edges(int blob[3][3]) {
  enum {x_axis = 0, y_axis = 1};
  int min_y = MIN_IDX(blob, y_axis, MIN_IDX(blob, y_axis, 0, 1), 2)
    /* smaller X dimention of the two points which are not the Y marker */
    , origin = MIN_IDX(blob, x_axis
		       , (min_y == 1 ? 0 : 1)
		       , (min_y == 2 ? 0 : 2));

  // printf("max_x:%d max_y:%d\n", max_x, max_y);

  blob[origin][2] = 0;
  blob[3 ^ (origin | min_y)][2] = 1;
  blob[min_y][2] = 2;
}

// @x_yellow	pixel x coordinate of yellow checker
// @y_yellow	pixel y coordinate of yellow checker
// @x_origin	pixel x coordinate of origin
// @y_origin	pixel y coordinate of origin
// @full_x		pixel x length of individual square
// @full_y		pixel y lenght of individual square
// Returns nothing - function calculates x and y coordinates of a yellow checker
Grid to_grid(int x_yellows[3], int y_yellows[3], int x_origin, int y_origin, int square_x, int square_y) {
  Grid g;
  int j;
	
  for (j = 0; j<3; j++) {
    g.at((y_yellows[j] - y_origin) / square_y
	 , (x_yellows[j] - x_origin) / square_x)--;
  }
  return g;
}

int main() {
  /* -------------------------------------------*/
  /* Declarations */
  Grid g;
  
  int i = 0,
    x_length,
    y_length,
    x, y
    , o_x, o_y, // 3 green blobs with x, y, and 0 or 1 (1 being is already associated w/ a coord)
    x_yellows[3],
    y_yellows[3];

  int blob[3][3]
    , tmp[3][3];
  	
  /* -------------------------------------------*/
	
  // Get most recent camera image and process it
  track_update();		
		
  /* -------------------------------------------*/
  /* GET ALL PIXEL COORDS OF 3 GREEN BLOBS */
	
  // Get x and y coordinate of 3 blobs
  // Set assignmed field to 0, meaning false
  for(i = 0; i < 3; ++i) {
    blob[i][0] = track_x(0,i);
    blob[i][1] = track_y(0,i);
    blob[i][2] = 0;
  }

  sort_edges(blob);
  /* // DEBUGGING - display coordinates of all blobs */
  /* for(i = 0; i < 3; ++i) { */
  /*   for(j = 0; j < 3; ++j) */
  /*     printf("%d ", blob[i][j]); */

  /*   printf("\n"); */
  /* } */

  memcpy(tmp, blob, sizeof(blob));

  for(i = 0; i < 3; ++i) memcpy(blob[ tmp[i][2] ], tmp[i], sizeof(blob[0]));

  /* -------------------------------------------*/
  /* CALCULATE Y PIXEL COORD OF TOP OF Y AXIS */
  o_x = blob[0][0];
  o_y = blob[0][1];

  x = blob[1][0];
  y = blob[2][1];
    
  /* DEBUGGING - prints origin x and y coordinates */
  printf("Origin: %d, %d \n\n", o_x, o_y);
		
  // Calculates x and y length of individual sqare on board
  x_length = square_length(o_x, x);
  y_length = square_length(o_y, y);

  /* DEBUGGING - if not equal, you must adjust green blobs */
  printf("The following averages should be equal.\n");
  printf("Average x length of square: %d\n", x_length);
  printf("Average y length of square: %d\n\n\n", y_length);


  // Gets x coordinates of 3 yellow checkers
  for ( i = 0; i<3; i++) {
    x_yellows[i]= track_x(1, i);
    y_yellows[i]= track_y(1, i);
  }

  for ( i = 0; i<3; i++)
    printf("Yellow (%d,%d)\n", x_yellows[i], y_yellows[i]);
		
  // Generates the x and y coordinates of a yellow checker
  g = to_grid(x_yellows, y_yellows, o_x, o_y, x_length, y_length);

  std::cout << g << std::endl;

  return 0;
}
