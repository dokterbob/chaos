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
#include "chaos.h"

void doubletochar(unsigned int size, double* buf, char* charbuf) {
	unsigned int i;
	double maxval=0., minval=0., scale;

	printf("Normalizing image...\n");

	// First find the heighest and the lowest value
	for (i=0; i<size; i++) {
		if (buf[i] > maxval) {
			maxval = buf[i];
			#ifdef DEBUG
				printf("New maximumum %f found at position %d\n", maxval, i);
			#endif
		}
		if (buf[i] < minval) {
			minval = buf[i];
			#ifdef DEBUG
				printf("New minimum %f found at position %d\n", minval, i);
			#endif		
		}
	}
	
	scale = 254./(maxval - minval);
	printf("Maximum value: %f\nMinimum value: %f\nScale factor: %f\n", maxval, minval, scale);

	for (i=0; i<size; i++) {
		charbuf[i] = (char)rint(buf[i]*scale);
	}
}

void writetiff(char* filename, int width, int height, char* buffer) {
	TIFF* image;

	printf("Writing %s...\n", filename);
	
	if((image = TIFFOpen(filename, "w")) == NULL) {
		printf("Could not open file for writing\n");
		exit(42);
	}

	// We need to set some values for basic tags before we can add any data
	TIFFSetField(image, TIFFTAG_IMAGEWIDTH, width);
	TIFFSetField(image, TIFFTAG_IMAGELENGTH, height);
	TIFFSetField(image, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(image, TIFFTAG_SAMPLESPERPIXEL, 1);
	TIFFSetField(image, TIFFTAG_ROWSPERSTRIP, height);
	
	// Ofter libtiff does not yet contain the LZW algorithm
	#ifdef LZW
	TIFFSetField(image, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
	#endif
	
	TIFFSetField(image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
	TIFFSetField(image, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
	TIFFSetField(image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

	TIFFSetField(image, TIFFTAG_XRESOLUTION, 72.0);
	TIFFSetField(image, TIFFTAG_YRESOLUTION, 72.0);
	TIFFSetField(image, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
  
	// Write the information to the file
	TIFFWriteEncodedStrip(image, 0, buffer, width * height);

	TIFFClose(image);
		
}

void duplicate_data(int width, int height, char* data) {
	char imagedata[width*height];
	
	int x, y, k;
	int halfway = width/2;

	printf("Mirroring image...\n");

	k=0;
	for (y=0; y<height; y++) {
		for (x=0; x<halfway; x++) {
			imagedata[y*width + halfway + x] = data[y*halfway + x];
			imagedata[y*width + halfway - x - 1] = data[y*halfway + x];
			k++;
		}
	}

	for (k=0; k<width*height; k++) {
		data[k] = imagedata[k];
	}
}

void execute(char* command) {
	printf("%s\n", command);
	system(command);		
}

void parse_opts(int argc, char **argv, calc_window* window) {
	int opt, option_index;

	char* err;
	
	static struct option long_options[] =
		{
			{"xres", required_argument, NULL, 'x'},
			{"yres", required_argument, NULL, 'y'},
			{"xmin", required_argument, NULL, 'a'},
			{"xmax", required_argument, NULL, 'b'},
			{"ymin", required_argument, NULL, 'c'},
			{"ymax", required_argument, NULL, 'd'},
			{"time-per-step", required_argument, NULL, 't'},
			{"steps", required_argument, NULL, 's'},
			{"tmax", required_argument, NULL, 'm'},
			{"help", no_argument, NULL, 'h'}
		};
	
	while ((opt = getopt_long(argc, argv, "w:h:a:b:c:d:t:s", long_options, &option_index)) != -1) {
		switch (opt) {	
			case 'x':
				window->width = strtol(optarg, &err, 10);
				break;

			case 'y':
				window->height = strtol(optarg, &err, 10);
				break;

			case 'a':
                                window->xmin = strtol(optarg, &err, 10);
				break;

                        case 'b':
                                window->xmax = strtol(optarg, &err, 10);
				break;

                        case 'c':
                                window->ymin = strtol(optarg, &err, 10);
				break;

                        case 'd':
                                window->ymin = strtol(optarg, &err, 10);
				break;

                        case 't':
                                window->tstep = strtod(optarg, &err);
				break;

                        case 's':
                                window->steps = strtol(optarg, &err, 10);
				break;

			case 'm':
				window->tmax = strtod(optarg, &err);
				break;

			default:
				// If none matched, make sure we set *err to something other than NULL
				err = malloc(sizeof(char));
				strcpy(err, "\1");
				break;
		}
	
		if (*err != '\0') {
			fprintf(stderr, "usage: %s --xres <pixels> --yres <pixels> --xmin <value> --xmax <value> --ymin <value> --ymax <value> --tmax <value> --time-per-step <value> --steps <value>\n", argv[0]);
                        exit(EXIT_FAILURE);
		}
	}

}

int main(int argc, char **argv) {
	unsigned short width, height;
	unsigned char wfactor;

	calc_window window;

	// Set defaults
	window.width = XRES;
	window.height = YRES;
	window.xmax = XMAX;
	window.xmin = XMIN;
	window.ymax = YMAX;
	window.ymin = YMIN;
	window.tstep = TSTEP;
	window.steps = TSTEP*STEPSS;
	window.tmax = TMAX;
	window.offset = 0;
	
	parse_opts(argc, argv, &window);

	printf("Producing image of %dx%d...\n", window.width, window.height);

	char imagedata[window.width*window.height];

        if (window.xmin == -window.xmax) {
                wfactor = 2;
		printf("Image symmetric, only calculating half.\n");
		window.xmin = 0.;
		window.width = window.width/wfactor;
        } else {
                wfactor = 1;
        }
	
	double buffer[window.width*window.height];

	printf("Allocating memory... ");
	calc_params*** data;
	
	int err_code = 0;
	char *free_ptr;
	char *base_ptr;
	
	unsigned int d[3] = {window.width, window.height, 4};
	int st[3] = {0,0,0};
	
	if ( (data = (calc_params***)daav(sizeof(calc_params), 3, d, st, &err_code, &free_ptr, NULL)) == NULL ) {
		printf("daav: error on dynamic	allocation. %s\n", daa_errs[err_code]);
	} 
	
	
	printf("done.\n");
		
	char basename[255];
	char tiffile[255];
	char bmpfile[255];
	char command[255];

	double t=0.;

	char endless;
	if (window.tmax < 0) {
		endless = 1;
		printf("\nStarted calculating until the end of time\n");
	} else {
		endless = 0;
		printf("\nStarted calculating until time is %f\n", window.tmax);
	}

	while (endless || t<window.tmax) {
		t = window.tstep*(window.offset+1);
		
		printf("\nFrame: %d Range: %f-%f\n", window.offset+1, window.tstep*window.offset, t);
		calc_image(buffer, data, &window);

		doubletochar(window.width * window.height, buffer, imagedata);
		
		if (wfactor == 2) duplicate_data(window.width*wfactor, window.height, imagedata);

		snprintf(basename, 255, "imgs/%.5d", window.offset);
		snprintf(tiffile, 255, "%s.tif", basename);
		snprintf(bmpfile, 255, "%s.bmp", basename);

		writetiff(tiffile, window.width*wfactor, window.height, imagedata);

#ifdef FINISH
		// This requires ImageMagick
		printf("Converting and scaling...\n");
		snprintf(command, 255, "convert -contrast -depth 24 -type truecolor -resize 50%% %s %s", tiffile, bmpfile);
		execute(command);

                snprintf(command, 255, "rm %s", tiffile);
		execute(command);
#endif
		window.offset++;
	}

	printf("\n%d frames written.\n", window.offset);
	printf("Done enough, dozing off for bed...\n");
							
			
	return EXIT_SUCCESS;
}

