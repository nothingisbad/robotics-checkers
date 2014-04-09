// Kaitlyn Carcia
// Last Updated April 8, 2014

// Returns length of each square on x axis
int x_axis(int x_origin, int x_right) {
	int x_axis = x_right - x_origin;
	int x = x_axis/8;
	return x;
}

// Returns length of each square of y axis
int y_axis(int y_origin, int y_top) {
	int y_axis = y_origin - y_top; 
	int y = y_axis/8;
	return y;
}

int main() 
{
	
	//	char board[2][2];
	// initialize board with all 0s
	//	int i, j;
	//	for (i = 0; i < 2; i++ ) {
	//		for (j = 0; j < 2; j++) {
	//			board[i][j] = '0';
	//		}
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
	avg_y;
	/* -------------------------------------------*/
	
	while(1) {
		// Get most recent camera image and process it
		track_update();		
		
		/* -------------------------------------------*/
		/* DEBUGGING */
		
		// Biggest Blob 0: Origin
		printf("Origin: (%d, %d)\n", track_x(0,0), track_y(0,0));
		
		// Mid Sized Blob 1: Top of y axis
		printf("Y axis: (%d, %d)\n", track_x(0,1), track_y(0,1));
		
		// Smallest Blob 2: Rightmost part of x axis
		printf("X axis: (%d, %d)\n\n", track_x(0,2), track_y(0,2));
		
		sleep(2.0);
		
		/* -------------------------------------------*/
		/* CALCULATE AVERAGE */
		
		// Arrays to store readings from camera
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
		
		
		if ( j == 2 ) { // When arrays are full
			for( k = 0; k < 3; k++ ) {				
				// Sum of origin_x
				sum_ox += origin_x[k];
				
				// Sum of origin_y
				sum_oy += origin_y[k];
				
				// Sum of x
				sum_x += x[k];
				
				// Sum of y
				sum_y += y[k];
			
				if ( k == 2 ) {
					// Calculates average for origin x and y, x, and y coordinates
					avg_ox = sum_ox/3;
					avg_oy = sum_oy/3;
					avg_x = sum_x/3;
					avg_y = sum_y/3;
					
					// Calculates individual square dimensions on x and y axes
					printf("Average square length on x axis: %d\n", x_axis(avg_ox, avg_x));
					printf("Average square length on y axis: %d\n\n", y_axis(avg_oy, avg_y));
					
					// Debugging
					/*printf("Avg OX: %d\n", avg_ox);
					printf("Avg OY: %d\n", avg_oy);
					printf("Avg X: %d\n", avg_x);
					printf("Avg Y: %d\n\n", avg_y);*/
				}
			}
		}
		j++; // Indicator for when arrays are full
	}
		/* -------------------------------------------*/
}
