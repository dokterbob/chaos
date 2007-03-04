/* Copyright (c)2006 Mathijs de Bruin
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
 
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

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
        unsigned int offset;
} calc_window;

// This the only parameter needed to be passed around to calculate
typedef struct {
	unsigned int x_i, y_i, k;

	unsigned int rows;

	double* buffer;
		
	calc_params*** data;
	calc_window* window;
	
} calc_data;

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
void calc_image(double* buffer, calc_params*** data, calc_window* window);

void calc_row(calc_data* p);
void *calc_thread(calc_data* p);
void prepare_data(calc_params*** data, calc_window* window);

