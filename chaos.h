#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <tiffio.h>

// Library for multi-dimensional efficient memory allocation
#include "daa.h"

// Numerical solver for 2-dimensional ODE's
#include "ndsolve.h"

// Parameters for the calculation of 
#include "params.h"

typedef struct {
	unsigned short width;
	unsigned short height;

        double xmin;
        double xmax;

        double ymin;
        double ymax;

        double tstep;
	double tmax;

        unsigned short steps;
        unsigned char offset;
} calc_window;

// Calculate the amount of chaos (predictability) for a given set of initial values
double calc_chaos(calc_window* params, calc_params* data);

// Set initial values for solving the ODE
void set_initial(double x, double y, calc_params* data);

// Calculate the amount of chaos using the function above for each pixel, and place the result in
// buffer[width*height], which should have been allocated. Buffer is written row after row with width
// elements.
// Here data is a three-dimensional array containing three steps of the calculation so we can calculate
// incrementally. The size of the dimensions of data are as follows:  data[width][height][4].
// The width and height stand for themselves, the 4 represents 0 for left of the requested coordinate,
// 1 for to the right, 2 for under and 3 for above. See set_initial for more info.
void calc_image(unsigned int width, unsigned int height, double* buffer, calc_params*** data, calc_window* window);

// This function rounds and normalizes a double array to an equal-sized char array. This needs to be
// done since trying to write floating point TIFF's so far has gotten me noting more than a severe
// headache.
void doubletochar(unsigned int size, double* buf, char* charbuf);

// Write a TIFF image to filename with dimensions width and height. Data read from buffer is places therein
// row-for-row.
void writetiff(char* filename, int width, int height, char* buffer);

// This function expects data to be filled with an image half the width we require. This image it mirrors to
// we get our full picture back. Was a nightmare to write...
void duplicate_data(int width, int height, char* data);

// Parse command line options using getopt_long and place the parsed options in calc_window
void parse_opts(int argc, char **argv, calc_window* window);

// system wrapper, printing each command executed 
void execute(char* command);

// The main program. Just read the code. 's Enough comments there.
int main(int argc, char **argv);

