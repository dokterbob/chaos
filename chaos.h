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
#include <getopt.h>
#include <string.h>
#include <tiffio.h>

// This is where the magic happens
#include "calc.h"

// Parameters for the calculation of 
#include "params.h"

// This function rounds and normalizes a double array to an equal-sized char array. This needs to be
// done since trying to write floating point TIFF's so far has gotten me noting more than a severe
// headache.
void doubletochar(unsigned int size, double* buf, char* charbuf);

// Write a TIFF image to filename with dimensions width and height. Data read from buffer is places therein
// row-for-row. A #define LZW 1 makes this bugger try and use LZW compression. Recommended if your libtiff
// supports it.
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

