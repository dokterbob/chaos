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

#define NTHREADS 3

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
        printf("Setting initial values for x=%d y=%d\n", x, y);
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

void calc_image(unsigned int width, unsigned int height, double* buffer, calc_params*** data, calc_window* window) {
	calc_data* p = malloc(sizeof(calc_data));
	
	// This shoudl be done from a higher level, off course
	p->width = width;
	p->height = height;
	p->buffer = buffer;
	p->data = data;
	p->window = window;
	
        printf("Calculating %dx%d pixels in %d steps: \n", p->width, p->height, p->window->steps);

        p->xstep = (p->window->xmax - p->window->xmin)/p->width;
        p->ystep = (p->window->ymax - p->window->ymin)/p->height;

        p->k = 0;
        p->y = p->window->ymin;
        for (p->y_i = 0; p->y_i < p->height; p->y_i++) {
                calc_row(p);

                // Print a dot for every row processed
                printf(".");
                fflush(stdout);
        }
        printf(" done\n");
}

void calc_row(calc_data* p) {
	p->x = p->window->xmin;
	for (p->x_i = 0; p->x_i < p->width; p->x_i++) {
		if (!p->window->offset) set_initial(p->x, p->y, p->data[p->x_i][p->y_i]);
		
		p->buffer[p->k] = calc_chaos(p->window, p->data[p->x_i][p->y_i]);
#ifdef DEBUG
		printf("Calculated x=%d y=%d: %f (%fx%f)\n", p->x_i, p->y_i, p->buffer[p->k], p->x, p->y);
#endif
		p->x += p->xstep;
		p->k++;
	}
	p->y += p->ystep;
}
