// Kaitlyn Carcia
// Last Updated April 9, 2014

// Function to calculate min and max of 2 numbers
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

// @x_origin	pixel x coordinate of origin
// @x_right		pixel x coordinate of rightmost part of the x axis
// Returns x length of every individual square
int x_axis(int x_origin, int x_right) {
	int x_axis = x_right - x_origin;
	int x = x_axis/8;
	return x;
}

// @y_origin	pixel y coordinate of origin
// @y_top		pixel y coordinate of the rightmost part of the y axis
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
// @full_x		pixel x length of individual square
// @full_y		pixel y lenght of individual square
// Returns nothing - function calculates x and y coordinates of a yellow checker
void to_grid(int x_yellow, int y_yellow, int x_origin, int y_origin, int full_x, int full_y) {
	// Debugging
	// printf("YELLOW: (%d, %d)\n\n", x_yellow, y_yellow);
	
	int x_diff = x_yellow - x_origin;
	int x_coord = (x_diff/full_x);
	
	int y_diff = y_origin - y_yellow;
	int y_coord = (y_diff/full_y);
	
	printf("(%d, %d)\n\n", x_coord, y_coord);
	sleep(2.0);
}

int main() 
{
	/* -------------------------------------------*/
	/* Declarations */
	
	int i = 0, // for loop
	x_yellow, // x coord for yellow checker piece
	y_yellow, // y coord for yellow checker piece
	x_length, // x length of an individual sqauare on the board
	y_length, // y length of an individual square on the board
	min_temp, min, max,  // min_temp, min, and max for calculate origin, x, and y axes
	blob1[3], blob2[3], blob3[3], // 3 green blobs with x, y, and 0 or 1 (1 being is already associated w/ a coord)
	x, // x coord of rightmost coord on the x axis
	y, // y coord of the top coord on the y axis
	o_x, o_y;  // x and y  coord of origin
	
	/* -------------------------------------------*/
	
	while(1) {
		// Get most recent camera image and process it
		track_update();		
		
		/* -------------------------------------------*/
		/* GET ALL PIXEL COORDS OF 3 GREEN BLOBS */
	
		// Get x and y coordinate of 3 blobs
		// Set assignned field to 0, meaning false
		blob1[0] = track_x(0,0);
		blob1[1] = track_y(0,0);
		blob1[2] = 0;
		
		blob2[0] = track_x(0,1);
		blob2[1] = track_y(0,1);
		blob2[2] = 0;
		
		blob3[0] = track_x(0,2);
		blob3[1] = track_y(0,2);
		blob3[2] = 0;
		

		// DEBUGGING - display coordinates of all blobs
		for (i=0; i<2; i++) {
			printf("%d ", blob1[i]);
		}
		printf("\n");
		sleep(0.5);
		
		for (i=0; i<2; i++) {
			printf("%d ", blob2[i]);
		}
		printf("\n");
		sleep(0.5);
		
		for (i=0; i<2; i++) {
			printf("%d ", blob3[i]);
		}
		printf("\n\n");
		sleep(0.5);
		
		/* -------------------------------------------*/
		/* CALCULATE Y PIXEL COORD OF TOP OF Y AXIS */
		
		// Calculate minimum y coordinate, which is the top of the y axis
		min_temp = MIN(blob1[1], blob2[1]);
		min = MIN( min_temp, blob3[1]);
		
		/* DEBUGGING - print min
		 printf("Min: %d\n", min);
		 sleep(0.5); */
		
		// Sets y coordinate, marks blob as assigned
		if (min == blob1[1]) {
			blob1[2] = 1; 
			y = blob1[1];
		} else if (min == blob2[1]) {
			blob2[2] = 1;
			y = blob2[1];
		} else if (min == blob3[1]) {
			blob3[2] = 1;
			y = blob3[1];
		}
		
		/* DEBEGGING - prints y coordinate, should be equal to min */
		printf("y coordinate of top is y axis is: %d\n\n", y);
		sleep(2.0);
		
		/* -------------------------------------------*/
		/* CALCULATE X COORD OF RIGHTMOST PART OF X AXIS */
		
		// Calculates maximum x coordinate, which is the rightmost part of the x axis
		if (blob1[2] == 1) {
			max = MAX(blob2[0], blob3[0]);
		} else if (blob2[2] == 1) {
			max = MAX(blob1[0], blob3[0]);
		} else if (blob3[2] == 1) {
			max = MAX(blob1[0], blob2[0]);
		}
		
		/* DEBUGGING - print max
		 printf("Max: %d\n", max);
		 sleep(0.5); */
		
		
		// Sets x coordinate to max, marks blob as assigned
		if (max == blob1[0]) {
			blob1[2] = 1; 
			x = blob1[0];
		} else if (max == blob2[0]) {
			blob2[2] = 1;
			x = blob2[0];
		} else if (max == blob3[0]) {
			blob3[2] = 1;
			x = blob3[0];
		}
		
		/* DEBUGGING - prints x coordinate, should be equal to max */
		printf("x coordinate of top is x axis is: %d\n\n", x);
		sleep(2.0);
		
		/* -------------------------------------------*/
		/* SETS LAST UNASSIGNMENT BLOB AS ORIGIN  */
		
		if ( blob1[2] == 0 ){
			o_x = blob1[0];
			o_y = blob1[1];
		} else if ( blob2[2] == 0 ){
			o_x = blob2[0];
			o_y = blob2[1];
		} else if ( blob3[2] == 0 ){
			o_x = blob3[0];
			o_y = blob3[1];
		}
		
		/* DEBUGGING - prints origin x and y coordinates */
		printf("Origin: %d, %d \n\n", o_x, o_y);
		sleep(2.0);
		
		// Calculates x and y length of individual sqare on board
		x_length = x_axis(o_x, x);
		y_length = y_axis(o_y, y);
					
		/* DEBUGGIN - if not equal, you must adjust green blobs */
		printf("The following averages should be equal.\n");
		printf("Average x length of square: %d\n", x_length);
		printf("Average y length of square: %d\n\n\n", y_length);
		sleep(3.0);		
					
		// Gets x and y coordinates of a single yellow checker
		x_yellow = track_x(1,0);
		y_yellow = track_y(1,0); 
		
		// Generates the x and y coordinates of a yellow checker
		to_grid(x_yellow, y_yellow, o_x, o_y, x_length, y_length);
		
		sleep(10.0);
	
	}
	/* -------------------------------------------*/
}
