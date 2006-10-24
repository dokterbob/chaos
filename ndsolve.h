#include <math.h>

/* The two motion equations being solved.
 * To accelerate for this specific problem I require the distance to the 
 * origin r and x respective y so I only have to calculate r once.
 *
 * This requires r = sqrt(x^2 + y^2).
 *
 * These equations describe a point mass (taken with m=1) attached to
 * a spring which is attached to the origin in 2D. This creates a
 * 'spring pendulum' producing chaotic behaviour. */
double a_x(double r, double x);
double a_y(double r, double y);

/* This function numerically solves a second-degree ODE for the motion 
 * equations given above.
 *
 * If offset is false it expects initial values to be set in x[0], y[0], 
 * vx[0] and vy[0]. In the other case it also expects the second and third 
 * element of these respective arrays to be available so as to make 
 * incremental calculations possible without any loss of precision.
 *
 * Steps is the number of steps that should be calculated, h the stepsize used.
 *
 * Actually, this function is nothing more than a 'redirection' to one of
 * the other three available solve functions. */
void solve_ode(unsigned short offset, unsigned short steps, double h, double x[3], double y[3], double vx[3], double vy[3], double ax[3], double ay[3]);

/* A helper function that moves the values in these arrays to the lower element
 * like so:
 * 1 -> 0: x[0] = x[1]
 * 2 -> 1: x[1] = x[2] */
void shuffle_vars(double x[3], double y[3], double vx[3], double vy[3], double ax[3], double ay[3]);

/* Debug function, printing the these variables, if STEPLOG is set.
 * Otherwise it does nothing. */
void pr_step(double x, double y, double vx, double vy, double ax, double ay);

/* This function amounts to a backward Euler approximation, originally written
 * to kickstart the other two approximations but then later extended to a full-
 * fledged solver. Parameters as in the solve_ode function. */
void euler(unsigned short offset, unsigned short steps, double h, double x[3], double y[3], double vx[3], double vy[3], double ax[3], double ay[3]);

/* This 'mysterious' method I found in Huang, T.Y. and Zhou Q.L.: 1993, A Numerical Method for Differential Equations of 
 * Second Order as published in Celestial Mechanics and Dynamical Astronomy 55: p 405-409 which can be found at
 * http://www.springerlink.com/content/h2281317764612p5/
 * For the first two steps I use Euler approximation. Parameters, once again, as in solve_ode. */
void method_a(unsigned short offset, unsigned short steps, double h, double x[3], double y[3], double vx[3], double vy[3], double ax[3], double ay[3]);

/* Adams-Bashford algorithm implemented on the basis of the syllabus 'Inleiding Numerieke Natuurkunde' (Dutch) as can be 
 * found on http://www.nikhef.nl/%7Eh73/numnat/docu/col-tot.pdf
 * Once again same parameters as solve_ode. */
void adams_bashford(unsigned short offset, unsigned short steps, double h, double x[3], double y[3], double vx[3], double vy[3], double ax[3], double ay[3]);


