#include "ndsolve.h"

// This function requires the initial place, time and velocity in the particle state to be given
float a_x(float r, float x) {
        return x*(1.-r)/r;
}

float a_y(float r, float y) {
        return y*(1.-r)/r - 1.;
}

void solve_ode(unsigned short offset, unsigned short steps, float h, float x[3], float y[3], float vx[3], float vy[3], float ax[3], float ay[3]) {
	adams_bashford(offset, steps, h, x, y, vx, vy, ax, ay);
}

void adams_bashford(unsigned short offset, unsigned short steps, float h, float x[3], float y[3], float vx[3], float vy[3], float ax[3], float ay[3]) {
        //printf("Solving ODE for x=%f, y=%f, %d steps of %f\n", x[0], y[0], steps, h);

        unsigned short k;

        float r;

        const float c1 = 3./2.;
        const float c2 = 1./2.;

	// If we start with an offset wo don't need to set this
	if (!offset) {
		//printf("Beginning: setting initial values\n");
        	// Set initial acceleration, x[0] and y[0] are supposed to be set - as are vx[0] and vy[0]
	        r = sqrtf(x[0]*x[0] + y[0]*y[0]);
        	ax[0] = a_x(r, x[0]);
        	ay[0] = a_y(r, y[0]);

                //printf("x=%f y=%f vx=%f vy=%f ax=%f ay=%f\n", x[0], y[0], vx[0], vy[0], ax[0], ay[0]);
		
       		// Set second values based on Euler approximation
        	x[1] = x[0] + h * vx[0];
        	y[1] = y[0] + h * vy[0];

                // Set the second acceleration
		//r = sqrt(x[1]*x[1] + y[1]*y[1]);
		//ax[1] = a_x(r, x[1]);
		//ay[1] = a_y(r, y[1]);
		
		vx[1] = vx[0] + h * ax[0];
        	vy[1] = vy[0] + h * ay[0];
  
		// Set the second acceleration
                r = sqrtf(x[1]*x[1] + y[1]*y[1]);
                ax[1] = a_x(r, x[1]);
                ay[1] = a_y(r, y[1]);

		//printf("x=%f y=%f vx=%f vy=%f ax=%f ay=%f\n", x[1], y[1], vx[1], vy[1], ax[1], ay[1]);
	}

        // Iterate the rest of this bitch using Adams-Bashforth
        for (k=2;k<steps; k++) {
                // Again, only use last (2) steps
                x[2] = x[1] + h * (c1 * vx[1] - c2 * vx[0]);
                y[2] = y[1] + h * (c1 * vy[1] - c2 * vy[0]);
                vx[2] = vx[1] + h * (c1 * ax[1] - c2 * ax[0]);
                vy[2] = vy[1] + h * (c1 * ay[1] - c2 * ay[0]);

                // Calculate the current acceleration from the current position
                r = sqrtf(x[2]*x[2] + y[2]*y[2]);
                ax[2] = a_x(r, x[2]);
                ay[2] = a_y(r, y[2]);

                //printf("x=%f y=%f vx=%f vy=%f ax=%f ay=%f\n", x[2], y[2], vx[2], vy[2], ax[2], ay[2]);

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

		//sleep(1);

        }
	//sleep(1);

}

