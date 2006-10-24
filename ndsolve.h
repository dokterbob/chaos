#include <math.h>

double a_x(double r, double x);
double a_y(double r, double y);

void solve_ode(unsigned short offset, unsigned short steps, double h, double x[steps], double y[steps], double vx[steps], double vy[steps], double ax[steps], double ay[steps]);

void adams_bashford(unsigned short offset, unsigned short steps, double h, double x[steps], double y[steps], double vx[steps], double vy[steps], double ax[steps], double ay[steps]);

void shuffle_vars(double x[3], double y[3], double vx[3], double vy[3], double ax[3], double ay[3]);

void euler(unsigned short offset, unsigned short steps, double h, double x[3], double y[3], double vx[3], double vy[3], double ax[3], double ay[3]);

void method_a(unsigned short offset, unsigned short steps, double h, double x[3], double y[3], double vx[3], double vy[3], double ax[3], double ay[3]);
