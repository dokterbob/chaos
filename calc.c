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
        unsigned int x_i, y_i, k;
        double xstep, ystep, x, y;

        printf("Calculating %dx%d pixels in %d steps: \n", width, height, window->steps);

        xstep = (window->xmax - window->xmin)/width;
        ystep = (window->ymax - window->ymin)/height;

        k=0;
        y=window->ymin;
        for (y_i=0; y_i<height; y_i++) {
                
                x=window->xmin;
                for (x_i=0; x_i<width; x_i++) {
                        if (!window->offset) set_initial(x, y, data[x_i][y_i]);
                        
                        buffer[k] = calc_chaos(window, data[x_i][y_i]);
                        #ifdef DEBUG
                        printf("Calculated x=%d y=%d: %f (%fx%f)\n", x_i, y_i, buffer[k], x, y);
                        #endif
                        x += xstep;
                        k++;
                }
                y += ystep;

                // Print a dot for every row processed
                printf(".");
                fflush(stdout);
        }
        printf(" done\n");
}

