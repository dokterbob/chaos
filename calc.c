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
#include "calc.h"

double calc_chaos(calc_window* params, calc_params* data) { 
        // Set the stepsize
        double h = params->tstep/params->steps;

        // Solve that ODE like you're bourne to do it! (*SHAKE it*!)
        int i;
        for (i=0; i<4; i++) {
                solve_ode(params->offset, params->steps, h, &data[i]);
        }

        // The latest calculated position will be in 1 and not 2 in the array with length
        // 3 because of the shuffling that takes place in solve_ode. (Please refer to that
        // code for more insight.)
        static const int cur = 1;
        double difx, dify;

        // One takes the absolute value of the difference between two calculated values,
        // corresponding thus to the derivate to x resp y when the ODE is seen as a 
        // functional. 
        difx = fabs(data[1].x[cur] - data[0].x[cur]);
        dify = fabs(data[3].y[cur] - data[2].y[cur]);

        return difx + dify;
}

void set_initial(double x, double y, calc_params* data) {
#ifdef DEBUG
        printf("Setting initial values for x=%f y=%f\n", x, y);
#endif
        
        // data[0] calculates just left from our point of interest
        data[0].x[0] = x - DX;
        data[0].y[0] = y;
        data[0].vx[0] = VX0;
        data[0].vy[0] = VY0;

        // data[1] calculates just right of it
        data[1].x[0] = x + DX;
        data[1].y[0] = y;
        data[1].vx[0] = VX0;
        data[1].vy[0] = VY0;

        // data[2] moves a tad bit under our point
        data[2].x[0] = x;
        data[2].y[0] = y - DY;
        data[2].vx[0] = VX0;
        data[2].vy[0] = VY0;

        // data[3] goes little above
        data[3].x[0] = x;
        data[3].y[0] = y + DY;
        data[3].vx[0] = VX0;
        data[3].vy[0] = VY0;
}

void calc_image(double* buffer, calc_params*** data, calc_window* window) {
        printf("Calculating %dx%d pixels in %d steps, using %d threads: \n", window->width, window->height, window->steps, NTHREADS);

	// Prepare the initial values
	if (!window->offset) {
		prepare_data(data, window);
	}

	
	pthread_t* thread = malloc(sizeof(pthread_t)*NTHREADS);
	calc_data** p = malloc(sizeof(calc_data*)*NTHREADS);
	
	div_t bla = div(window->height, NTHREADS);
	unsigned int batchsize = bla.quot;
	
	//int batchsize = div(window->height, NTHREADS).quot;

	#ifdef DEBUG
	printf("Batchsize: (%d/%d) = %d + %d\n", window->height, NTHREADS, batchsize, bla.rem);
	#endif
	
	// Start threads
	int i;
	for (i=0; i < NTHREADS; i++) {
		// Dynamic allocation, bitch! (Yes, we can have any dynamic number of threads)
		p[i] = malloc(sizeof(calc_data));
		
		p[i]->buffer = buffer;
		p[i]->data = data;
		p[i]->window = window;
		
		p[i]->k = i*batchsize*window->width;
		p[i]->y_i = i*batchsize;
		
		if (i != (NTHREADS-1)) {
			p[i]->rows = batchsize;
		} else {
			p[i]->rows = window->height - i*batchsize;
		}
		
		printf("Thread %d doing rows %d to %d\n", i, p[i]->y_i, p[i]->y_i+p[i]->rows-1);
		#ifdef DEBUG
		printf("k runs from %d to %d\n", p[i]->k, p[i]->k+p[i]->rows*window->width);
		#endif


		pthread_create(&thread[i], NULL, (void *)calc_thread, p[i]);
	}
	
	// Stick 'em together
	int j;
	for (j=0; j < NTHREADS; j++) {
		pthread_join(thread[j], NULL);
	}
	
	// When done, clean up
	for (j=0; j < NTHREADS; j++) {
		free(p[j]);
	}

	free(p);
	
        printf(" done\n");
}

void prepare_data(calc_params*** data, calc_window* window) {
	double x, y;
	unsigned int x_i, y_i;
	
	double xstep, ystep;
	
	xstep = (window->xmax - window->xmin)/window->width;
        ystep = (window->ymax - window->ymin)/window->height;
	
	y = window->ymin;
	for (y_i = 0; y_i < window->height; y_i++) {
		x = window->xmin;
		
		for (x_i = 0; x_i < window->width; x_i++) {
			set_initial(x, y, data[x_i][y_i]);
			x += xstep;
		}
		y += ystep;
	}
}

void *calc_thread(calc_data* p) {
	unsigned int stopval;
	stopval = p->y_i + p->rows;
	
        for (; p->y_i <  stopval; p->y_i++) {
		p->x_i = 0;
             		
		#ifndef DEBUG
                // Print a dot for every row processed
                printf(".");
                fflush(stdout);
		#endif
		
		#ifdef DEBUG
		printf("Calculating line %d\n", p->y_i);
		#endif
		
		calc_row(p);
        }
}

void calc_row(calc_data* p) {
	for (p->x_i; p->x_i < p->window->width; p->x_i++) {

		p->buffer[p->k] = calc_chaos(p->window, p->data[p->x_i][p->y_i]);
#ifdef DEBUG
		printf("Calculated x=%d y=%d (%d) : %f\n", p->x_i, p->y_i, p->k, p->buffer[p->k]);
#endif
		p->k++;
	}
}
