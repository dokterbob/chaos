#include <math.h>

float a_x(float r, float x);
float a_y(float r, float y);

void solve_ode(unsigned short offset, unsigned short steps, float h, float x[steps], float y[steps], float vx[steps], float vy[steps], float ax[steps], float ay[steps]);

void adams_bashford(unsigned short offset, unsigned short steps, float h, float x[steps], float y[steps], float vx[steps], float vy[steps], float ax[steps], float ay[steps]);

