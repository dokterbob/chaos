#include "ndsolve.h"

// This function requires the initial place, time and velocity in the particle state to be given
double a_x(double r, double x) {
        return x*(1.-r)/r;
}

double a_y(double r, double y) {
        return y*(1.-r)/r - 1.;
}

void solve_ode(unsigned short offset, unsigned short steps, double h, double x[3], double y[3], double vx[3], double vy[3], double ax[3], double ay[3]) {
	//adams_bashford(offset, steps, h, x, y, vx, vy, ax, ay);
	//euler(offset, steps, h, x, y, vx, vy, ax, ay);
        method_a(offset, steps, h, x, y, vx, vy, ax, ay);
}

void shuffle_vars(double x[3], double y[3], double vx[3], double vy[3], double ax[3], double ay[3]) {
                // Move over the variables
                x[0] = x[1];
                y[0] = y[1];
                vx[0] = vx[1];
                vy[0] = vy[1];
                ax[0] = ax[1];
                ay[0] = ay[1];

                x[1] = x[2];
                y[1] = y[2];
                vx[1] = vx[2];
                vy[1] = vy[2];
                ax[1] = ax[2];
                ay[1] = ay[2];
}

void pr_step(double x, double y, double vx, double vy, double ax, double ay) {
#ifdef STEPLOG
	printf("x=%f y=%f vx=%f vy=%f ax=%f ay=%f\n", x, y, vx, vy, ax, ay);
	sleep(1);
#endif
}

// Backward Euler approximation
void euler(unsigned short offset, unsigned short steps, double h, double x[3], double y[3], double vx[3], double vy[3], double ax[3], double ay[3]) {
	double r;
	unsigned short k=0;

	// Calculate the first value, if needed
	if (!offset) {
                x[1] = x[0] + h * vx[0];
                y[1] = y[0] + h * vy[0];

                vx[1] = vx[0] + h * ax[0];
                vy[1] = vy[0] + h * ay[0];

                r = sqrt(x[1]*x[1] + y[1]*y[1]);

                ax[1] = a_x(r, x[1]);
                ay[1] = a_y(r, y[1]);

		pr_step(x[1], y[1], vx[1], vy[1], ax[1], ay[1]);

		k++;
	}

	for(;k<steps;k++) {
                x[2] = x[1] + h * vx[1];
                y[2] = y[1] + h * vy[1];

                vx[2] = vx[1] + h * ax[1];
                vy[2] = vy[1] + h * ay[1];

                r = sqrt(x[2]*x[2] + y[2]*y[2]);

                ax[2] = a_x(r, x[2]);
                ay[2] = a_y(r, y[2]);

                pr_step(x[2], y[2], vx[2], vy[2], ax[2], ay[2]);

		shuffle_vars(x, y, vx, vy, ax, ay);
	}

}

void method_a(unsigned short offset, unsigned short steps, double h, double x[3], double y[3], double vx[3], double vy[3], double ax[3], double ay[3]) {
        double k1x, k2x, k3x, k1y, k2y, k3y;
	double tx, ty;
	double sx, sy;
	double r;

	double k;

	/*
        define ha 0.01
        define h_2 (1./2.)*ha
        define h_48 (1./48.)*ha*ha

        define h2 2*ha
        define h4_3 (4./3.)*ha

        define h_1_3 (1./3.)*ha*ha
	*/

	const double h_2 = (1./2.)*h;
	const double h_48 = (1./48.)*pow(h, 2.);

	const double h2 = 2*h;
	const double h4_3 = (4./3.)*h;

	const double h_1_3 = (1./3.)*pow(h, 2.);
	
	double qx, qy, px, py, rx, ry;

        if (!offset) {
                euler(0, 1, h, x, y, vx, vy, ax, ay);
        }

        // Iterate the rest of this bitch using Adams-Bashforth
        for (k=2;k<=steps+offset; k++) {
        	k1x = ax[1];
        	k1y = ay[1];

		qx = x[1] - h_2*vx[1];
		qy = y[1] - h_2*vy[1];

		px = h_48*ax[0];
		py = h_48*ay[0];

		rx = h_48*ax[1];
		ry = h_48*ay[1];

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

        	vx[2] = vx[0] - h2 * k1x + h4_3 * sx;
        	x[2] = 2 * x[1] - x[0] + h_1_3 * sx;

        	vy[2] = vy[0] - h2 * k1y + h4_3 * sy;
        	y[2] = 2 * y[1] - y[0] + h_1_3 * sy;

                pr_step(x[2], y[2], vx[2], vy[2], ax[2], ay[2]);

                shuffle_vars(x, y, vx, vy, ax, ay);
        }
}

void adams_bashford(unsigned short offset, unsigned short steps, double h, double x[3], double y[3], double vx[3], double vy[3], double ax[3], double ay[3]) {
        //printf("Solving ODE for x=%f, y=%f, %d steps of %f\n", x[0], y[0], steps, h);

        unsigned short k;

        const double c1 = 3./2.;
        const double c2 = 1./2.;

	double r;

	// If we start with an offset wo don't need to set this
	if (!offset) {
		euler(0, 1, h, x, y, vx, vy, ax, ay);
	}

        // Iterate the rest of this bitch using Adams-Bashforth
        for (k=2;k<=steps+offset; k++) {
                // Again, only use last (2) steps
                x[2] = x[1] + h * (c1 * vx[1] - c2 * vx[0]);
                y[2] = y[1] + h * (c1 * vy[1] - c2 * vy[0]);
                vx[2] = vx[1] + h * (c1 * ax[1] - c2 * ax[0]);
                vy[2] = vy[1] + h * (c1 * ay[1] - c2 * ay[0]);

                // Calculate the current acceleration from the current position
                r = sqrt(x[2]*x[2] + y[2]*y[2]);
                ax[2] = a_x(r, x[2]);
                ay[2] = a_y(r, y[2]);

                pr_step(x[2], y[2], vx[2], vy[2], ax[2], ay[2]);

		shuffle_vars(x, y, vx, vy, ax, ay);
        }

}

