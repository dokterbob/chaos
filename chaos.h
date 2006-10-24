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
        double t;

        unsigned short steps;
        unsigned char offset;
} ode_params;

typedef struct {
	unsigned short width;
	unsigned short height;

        double xmin;
        double xmax;

        double ymin;
        double ymax;

        double t;
	double tmax;

        unsigned short steps;
        unsigned char offset;
} calc_window;

// Calculate the amount of chaos for a given set of initial values
double calc_chaos(ode_params* params, calc_params* data);

void calc_image(unsigned int width, unsigned int height, double* buffer, calc_params*** data, calc_window* window);

void doubletochar(unsigned int size, double* buf, char* charbuf);

void writetiff(char* filename, int width, int height, char* buffer);

void duplicate_data(int width, int height, char* data);

void parse_opts(int argc, char **argv, calc_window* window);

void execute(char* command);

int main();

