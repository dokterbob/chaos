#include "chaos.h"

typedef struct {
	double x0;
	double y0;

	double vx0;
	double vy0;

	double t;

	unsigned short steps;
	unsigned short offset;
} ode_params;

typedef struct {
        double x[3];
        double y[3];
        double vx[3];
        double vy[3];
        double ax[3];
        double ay[3];
} calc_params;

// Dit is rotzooi !!!
double calc_chaos(ode_params* params, calc_params data[4]) { 
        // The step size for calculation
        double dx = 0.01;
        double dy = 0.01;

	// Set initial values
	if (!params->offset) {
		data[0].x[0] = params->x0 - dx;
		data[0].y[0] = params->y0;
		data[0].vx[0] = params->vx0;
		data[0].vy[0] = params->vy0;

                data[1].x[0] = params->x0 + dx;
                data[1].y[0] = params->y0;
                data[1].vx[0] = params->vx0;
                data[1].vy[0] = params->vy0;

                data[2].x[0] = params->x0;
                data[2].y[0] = params->y0 - dy;
                data[2].vx[0] = params->vx0;
                data[2].vy[0] = params->vy0;

                data[3].x[0] = params->x0;
                data[3].y[0] = params->y0 + dy;
                data[3].vx[0] = params->vx0;
                data[3].vy[0] = params->vy0;
	}

	// Set the stepsize
	double h = params->t/params->steps;

	int i;
	for (i=0; i<4; i++) {
		//printf("data[%d]\n",i);
		solve_ode(params->offset, params->steps, h, data[i].x, data[i].y, data[i].vx, data[i].vy, data[i].ax, data[i].ay);
	}

	const int cur = 1;
	double difx, dify;

	//printf("x1=%f, x2=%f, y1=%f, y2=%f\n", data[1].x[cur], data[0].x[cur], data[3].y[cur], data[2].y[cur]);

	difx = data[1].x[cur] - data[0].x[cur];
	dify = data[3].y[cur] - data[2].y[cur];

	//printf("x[last] = %f\n", data[0].x[last]);
	return fabs(difx) + fabs(dify);
}

typedef struct {
	double xmin;
	double xmax;

	double ymin;
	double ymax;

	double t;

	unsigned short steps;
	unsigned short offset;
} calc_window;

void calc_image(unsigned int width, unsigned int height, double* buffer, calc_params*** data, calc_window* window) {
	unsigned int x_i, y_i, k;
	double xstep, ystep, x, y;

	printf("Calculating %dx%d pixels in %d steps: \n", width, height, window->steps);

	ode_params params;

	xstep = (window->xmax - window->xmin)/width;
	ystep = (window->ymax - window->ymin)/height;

	params.t = window->t;
	params.steps = window->steps;
	params.vx0 = 0.;
	params.vy0 = 0.;
	params.offset = window->offset;

	k=0;
	y=window->ymin;
	for (y_i=0; y_i<height; y_i++) {
		x=window->xmin;
		for (x_i=0; x_i<width; x_i++) {
			params.x0 = x;
			params.y0 = y;
			
			buffer[k] = calc_chaos(&params, data[x_i][y_i]);

			//printf("Calculating x=%d y=%d: %f (%fx%f)\n", x_i, y_i, buffer[k], x, y);

			x += xstep;
			k++;
		}
		y += ystep;
		printf(".");
		fflush(stdout);
	}

	printf(" done\n");
}

void doubletochar(unsigned int size, double* buf, char* charbuf) {
	unsigned int i;
	double maxval=0., minval=0., scale;

	printf("Converting to image...\n");

	// First find the heighest and the lowest value
	for (i=0; i<size; i++) {
		if (buf[i] > maxval) maxval = buf[i];
		if (buf[i] < minval) minval = buf[i];
	}
	
	scale = 254./(maxval - minval);
	printf("Maximum value: %f\nMinimum value: %f\nScale factor: %f\n", maxval, minval, scale);

	printf("Setting char array...\n");
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

	printf("Mirroring image: %d = 2x%d.\n",width*height, halfway);

	k=0;
	for (y=0; y<height; y++) {
		for (x=0; x<halfway; x++) {
			//printf("data[%d] = data[%d]\n",y*width + halfway + x,y*halfway + x);
			imagedata[y*width + halfway + x] = data[y*halfway + x];
			imagedata[y*width + halfway - x - 1] = data[y*halfway + x];
			k++;
		}
	}

	for (k=0; k<width*height; k++) {
		data[k] = imagedata[k];
	}
}

int main() {
	int width, height;
	
	width = XRES;
	height = YRES;

	printf("Producing image of %dx%d...\n", width, height);

	double buffer[width*height];
	char imagedata[width*height];

	calc_window window;

        window.xmin = XMIN;
        window.xmax = XMAX;
        window.ymin = YMIN;
        window.ymax = YMAX;
        window.t = TSTEP;
        window.steps = TSTEP*STEPSS;
	window.offset = 0;

	printf("Allocating memory... ");
	calc_params*** data;
	
	int err_code = 0;
	char *free_ptr;
	char *base_ptr;
	
	int d[3] = {width, height, 4};
	int st[3] = {0,0,0};
	
	if ( (data = (calc_params***)daav(sizeof(calc_params), 3, d, st, &err_code, &free_ptr, NULL)) == NULL ) {
		printf("daav: error on dynamic	allocation. %s\n", daa_errs[err_code]);
	} 
	
	
	printf("done.\n");
		
	int i;
	int imax=2;
	char basename[255];
	char tiffile[255];
	char bmpfile[255];
	char command[255];

	for (i=1; i<imax; i++) {
		printf("Frame: %d Range: %f-%f\n", i, window.t*(i-1), window.t*i);
		calc_image(width/2, height, buffer, data, &window);

		doubletochar(width * height/2, buffer, imagedata);
		duplicate_data(width, height, imagedata);

		snprintf(basename, 255, "imgs/%.5d", i-1);
		snprintf(tiffile, 255, "%s.tif", basename);
		snprintf(bmpfile, 255, "%s.bmp", basename);

		writetiff(tiffile, width, height, imagedata);

		printf("Converting and scaling...\n");
		snprintf(command, 255, "convert -contrast -depth 24 -type truecolor -resize 50%% %s %s", tiffile, bmpfile);

		printf("%s\n", command);
		system(command);

		snprintf(command, 255, "scp -C %s drbob@whale.dokterbob.net:/var/files/Unsorted/Temp/chaos/", bmpfile);
                printf("%s\n", command);
                system(command);

                snprintf(command, 255, "rm %s %s", tiffile, bmpfile);
                printf("%s\n", command);
                system(command);
		window.offset = 1;
	}
			
	return 0;
}

