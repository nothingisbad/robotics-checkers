// Kaitlyn Carcia
// Last Updated April 9, 2014

// @x_origin	pixel x coordinate of origin
// @x_right	pixel x coordinate of rightmost part of the x axis
// Returns x length of every individual square
int x_axis(int x_origin, int x_right) {
  int x_axis = x_right - x_origin;
  int x = x_axis/8;
  return x;
}

// @y_origin	pixel y coordinate of origin
// @y_top	pixel y coordinate of the rightmost part of the y axis
// Returns y length of every individual square
int y_axis(int y_origin, int y_top) {
  int y_axis = y_origin - y_top; 
  int y = y_axis/8;
  return y;
}

// @x_yellow	pixel x coordinate of yellow checker
// @y_yellow	pixel y coordinate of yellow checker
// @x_origin	pixel x coordinate of origin
// @y_origin	pixel y coordinate of origin
// @full_x	pixel x length of individual square
// @full_y	pixel y lenght of individual square

// Returns nothing - function calculates x and y coordinates of a yellow checker
void to_grid(int x_yellow, int y_yellow, int x_origin, int y_origin, int full_x, int full_y) {
  // Debugging
  // printf("YELLOW: (%d, %d)\n\n", x_yellow, y_yellow);

  int x_diff = x_yellow - x_origin;
  int x_coord = (x_diff/full_x);
  int y_diff = y_yellow - y_origin;
  int y_coord = (y_diff/full_y);
  printf("(%d, %d)\n\n", x_coord, y_coord);
  sleep(2.0);
}

int main() 
{
  //	char board[2][2];
  // initialize board with all 0s
  //	int i, j;
  //	for (i = 0; i < 2; i++ ) {
  //	for (j = 0; j < 2; j++) {
  //	board[i][j] = '0';
  //	}
  //	}

  /* -------------------------------------------*/
  /* Declarations */
  int i = 0,
    j = 0,
    k,
    sum_ox = 0,
    sum_oy = 0,
    sum_x = 0,
    sum_y = 0,
    avg_ox,
    avg_oy,
    avg_x,
    avg_y,
    x_yellow,
    y_yellow,
    full_x,
    full_y;

  /* -------------------------------------------*/
  while(1) {
    // Get most recent camera image and process it
    track_update();	

    /* -------------------------------------------*/
    /* DEBUGGING */
    // Biggest Blob 0: Origin
    // printf("Origin: (%d, %d)\n", track_x(0,0), track_y(0,0));
    // Mid Sized Blob 1: Top of y axis
    // printf("Y axis: (%d, %d)\n", track_x(0,1), track_y(0,1));
    // Smallest Blob 2: Rightmost part of x axis
    // printf("X axis: (%d, %d)\n\n", track_x(0,2), track_y(0,2));
    // sleep(2.0);

    /* -------------------------------------------*/
    /* CALCULATE AVERAGE */
    // Arrays to store x and y readings from camera
    int origin_x[3],
      origin_y[3],
      x[3],
      y[3];

    // Populate arrays with values
    if ( i < 3 ) {
      origin_x[i] = track_x(0,0);
      origin_y[i] = track_y(0,0);
      x[i] = track_x(0,2);
      y[i] = track_y(0,1);
    }
    i++; // Allows for 3 image captures

    // Might be able to just do i == 2 here?
    if ( j == 2 ) { // When arrays are full
      for( k = 0; k < 3; k++ ) {	

	// Sum of origin_x readings
	sum_ox += origin_x[k];

	// Sum of origin_y readings
	sum_oy += origin_y[k];

	// Sum of x readings
	sum_x += x[k];

	// Sum of y readings
	sum_y += y[k];

	// When all sums have finished calculating
	if ( k == 2 ) {

	  // Calculates average for origin x and y, x, and y coordinates
	  avg_ox = sum_ox/3;
	  avg_oy = sum_oy/3;
	  avg_x = sum_x/3;
	  avg_y = sum_y/3;

	  // Calculates x and y length of individual sqare on board
	  full_x = x_axis(avg_ox, avg_x);
	  full_y = y_axis(avg_oy, avg_y);

	  // Debugging - if not equal, you must adjust green blobs
	  printf("The following averages should be equal.\n");
	  printf("Average x length of square: %d\n", full_x);
	  printf("Average y length of square: %d\n\n\n", full_y);
	  sleep(3.0);

	  // Debugging - shows avg ox, oy, x, and y coordinates
	  /*printf("Avg OX: %d\n", avg_ox);
	    printf("Avg OY: %d\n", avg_oy);
	    printf("Avg X: %d\n", avg_x);
	    printf("Avg Y: %d\n\n", avg_y);*/

	  // Generates the x and y coordinates of a yellow checker
	  to_grid(x_yellow, y_yellow, avg_ox, avg_oy, full_x, full_y);
	}
      }
    }
    j++; // Indicator for when 3 array captures occured
 
   // We could do an average here, but I don't think it's necessary
    // Gets x and y coordinates of a single yellow checker
    x_yellow = track_x(1,0);
    y_yellow = track_y(1,0); 
  }
  /* -------------------------------------------*/
}
