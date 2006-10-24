#include <math.h>
#include <stdio.h>
#include <tiffio.h>
#include "daa.h"
#include "ndsolve.h"
#include "params.h"

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

typedef struct {
        double xmin;
        double xmax;

        double ymin;
        double ymax;

        double t;

        unsigned short steps;
        unsigned short offset;
} calc_window;

double calc_chaos(ode_params* params, calc_params data[4]);

void calc_image(unsigned int width, unsigned int height, double* buffer, calc_params*** data, calc_window* window);

void doubletochar(unsigned int size, double* buf, char* charbuf);

void writetiff(char* filename, int width, int height, char* buffer);

void duplicate_data(int width, int height, char* data);

int main();

