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
#include "ndsolve.h"

// This function requires the initial place, time and velocity in the particle state to be given
double a_x(double r, double x) {
        return x*(1.-r)/r;
}

double a_y(double r, double y) {
        return y*(1.-r)/r - 1.;
}

void solve_ode(unsigned char offset, unsigned short steps, double h, calc_params* data) {
	adams_bashford(offset, steps, h, data);
	//euler(offset, steps, h, data);
        //method_a(offset, steps, h, data);
}

void shuffle_vars(calc_params* data) {
                // Move over the variables
                data->x[0] = data->x[1];
                data->y[0] = data->y[1];
                data->vx[0] = data->vx[1];
                data->vy[0] = data->vy[1];
                data->ax[0] = data->ax[1];
                data->ay[0] = data->ay[1];

                data->x[1] = data->x[2];
                data->y[1] = data->y[2];
                data->vx[1] = data->vx[2];
                data->vy[1] = data->vy[2];
                data->ax[1] = data->ax[2];
                data->ay[1] = data->ay[2];
}

void pr_step(calc_params* data, unsigned char n) {
#ifdef STEPLOG
	printf("x=%f y=%f vx=%f vy=%f ax=%f ay=%f\n", data->x[n], data->y[n], data->vx[n], data->vy[n], data->ax[n], data->ay[n]);
	sleep(1);
#endif
}

// Backward Euler approximation
void euler(unsigned char offset, unsigned short steps, double h, calc_params* data) {
#ifdef STEPLOG
	printf("Starting Euler...\n");
#endif
	double r;
	unsigned short k=0;

	// Calculate the first value, if needed
	if (!offset) {
                data->x[1] = data->x[0] + h * data->vx[0];
                data->y[1] = data->y[0] + h * data->vy[0];

                data->vx[1] = data->vx[0] + h * data->ax[0];
                data->vy[1] = data->vy[0] + h * data->ay[0];

                r = sqrt(data->x[1]*data->x[1] + data->y[1]*data->y[1]);

                data->ax[1] = a_x(r, data->x[1]);
                data->ay[1] = a_y(r, data->y[1]);

		k++;
	}

	pr_step(data, 0);
	pr_step(data, 1);
	
	for(;k<steps;k++) {
                data->x[2] = data->x[1] + h * data->vx[1];
                data->y[2] = data->y[1] + h * data->vy[1];

                data->vx[2] = data->vx[1] + h * data->ax[1];
                data->vy[2] = data->vy[1] + h * data->ay[1];

                r = sqrt(data->x[2]*data->x[2] + data->y[2]*data->y[2]);

                data->ax[2] = a_x(r, data->x[2]);
                data->ay[2] = a_y(r, data->y[2]);

                pr_step(data, 2);

		shuffle_vars(data);
	}

}

void method_a(unsigned char offset, unsigned short steps, double h, calc_params* data) {
#ifdef STEPLOG
	        printf("Starting Method-A...\n");
#endif
		
        double k1x, k2x, k3x, k1y, k2y, k3y;
	double tx, ty;
	double sx, sy;
	double r;

	double k;

	const double h_2 = (1./2.)*h;
	const double h_48 = (1./48.)*pow(h, 2.);

	const double h2 = 2*h;
	const double h4_3 = (4./3.)*h;

	const double h_1_3 = (1./3.)*pow(h, 2.);
	
	double qx, qy, px, py, rx, ry;

        if (!offset) {
                euler(0, 1, h, data);
        }

        // Iterate the rest of this bitch using Adams-Bashforth
        for (k=2;k<=steps+offset; k++) {
        	k1x = data->ax[1];
        	k1y = data->ay[1];

		qx = data->x[1] - h_2*data->vx[1];
		qy = data->y[1] - h_2*data->vy[1];

		px = h_48*data->ax[0];
		py = h_48*data->ay[0];

		rx = h_48*data->ax[1];
		ry = h_48*data->ay[1];

        	tx = qx + 5*rx + px;
        	ty = qy + 5*ry + py;
        	r = sqrt(tx*tx + ty*ty);
        	k2x = a_x(r, tx);
        	k2y = a_y(r, ty);

        	tx = qx + 7*rx + px;
        	ty = qy + 7*ry + py;
        	r = sqrt(tx*tx + ty*ty);
        	k3x = a_x(r, tx);
        	k3y = a_y(r, ty);

        	sx = k1x + k2x + k3x;
        	sy = k1y + k2y + k3y;

        	data->vx[2] = data->vx[0] - h2 * k1x + h4_3 * sx;
        	data->x[2] = 2 * data->x[1] - data->x[0] + h_1_3 * sx;

        	data->vy[2] = data->vy[0] - h2 * k1y + h4_3 * sy;
        	data->y[2] = 2 * data->y[1] - data->y[0] + h_1_3 * sy;

                pr_step(data, 2);

                shuffle_vars(data);
        }
}

void adams_bashford(unsigned char offset, unsigned short steps, double h, calc_params *data) {
#ifdef STEPLOG
	        printf("Starting Adams-Bashford...\n");
#endif
		
        unsigned short k;

        const double c1 = 3./2.;
        const double c2 = 1./2.;

	double r;

	// If we start with an offset wo don't need to set this
	if (!offset) {
		euler(0, 1, h, data);
	}

        // Iterate the rest of this bitch using Adams-Bashforth
        for (k=2;k<=steps+offset; k++) {
                // Again, only use last (2) steps
                data->x[2] = data->x[1] + h * (c1 * data->vx[1] - c2 * data->vx[0]);
                data->y[2] = data->y[1] + h * (c1 * data->vy[1] - c2 * data->vy[0]);
                data->vx[2] = data->vx[1] + h * (c1 * data->ax[1] - c2 * data->ax[0]);
                data->vy[2] = data->vy[1] + h * (c1 * data->ay[1] - c2 * data->ay[0]);

                // Calculate the current acceleration from the current position
                r = sqrt(data->x[2]*data->x[2] + data->y[2]*data->y[2]);
                data->ax[2] = a_x(r, data->x[2]);
                data->ay[2] = a_y(r, data->y[2]);

                pr_step(data, 2);

		shuffle_vars(data);
        }

}

