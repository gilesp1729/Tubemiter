#include <stdio.h>
#include "dxf.h"
#include <math.h>
#include <stdlib.h>

#define PI      3.1415926
#define N       20

static int dxf_layer = 0;

FILE *
dxf_open
(
    char        *filename
)
{
    FILE    *file = fopen(filename, "w");

    dxf_layer = 0;
    fprintf(file, "0\nSECTION\n");
    fprintf(file, "2\nHEADER\n");
    fprintf(file, "9\n$MEASUREMENT\n");
    fprintf(file, "70\n1\n");
    fprintf(file, "9\n$INSUNITS\n");
    fprintf(file, "70\n4\n");
    fprintf(file, "0\nENDSEC\n");
    
    fprintf(file, "0\nSECTION\n");
    fprintf(file, "2\nENTITIES\n");

    return file;
}


void
dxf_close
(
    FILE	*file
)
{
    fprintf(file, "0\nENDSEC\n");
    fprintf(file, "0\nEOF\n");
    fclose(file);
}


void
dxf_set_layer
(
    int     layer
)
{
    dxf_layer = layer;
}


void
dxf_line
(
    FILE	*file,
    double	x1,
    double	y1,
    double	x2,
    double	y2
)
{
    fprintf(file, "0\nLINE\n");
    fprintf(file, "8\n%d\n", dxf_layer);
    fprintf(file, "10\n%lf\n", x1);
    fprintf(file, "20\n%lf\n", y1);
    fprintf(file, "11\n%lf\n", x2);
    fprintf(file, "21\n%lf\n", y2);
}


void
dxf_polyline
(
    FILE	*file,
    int	        n_pts,
    double	*x,
    double	*y
)
{
    int         i;

    fprintf(file, "0\nPOLYLINE\n");
    fprintf(file, "8\n%d\n", dxf_layer);
    
    for (i = 0; i < n_pts; i++)
    {
        fprintf(file, "0\nVERTEX\n");
        fprintf(file, "10\n%lf\n", x[i]);
        fprintf(file, "20\n%lf\n", y[i]);
    }

    fprintf(file, "0\nSEQEND\n");
}


void
dxf_polybezier
(
    FILE	*file,
    int	        n_pts,
    double	*x,
    double	*y
)
{
    int         i, j;
    double      t, t1;
    double      xb[N];
    double      yb[N];

    for (j = 0; j < n_pts - 1; j += 3)
    {
        for (i = 0, t = 0; i < N; i++, t += 1.0/(N-1))
        {
            t1 = 1 - t;
            xb[i] = 
                x[0]*t1*t1*t1
                +
                x[1]*t*t1*t1*3
                +
                x[2]*t*t*t1*3
                +
                x[3]*t*t*t;
            yb[i] = 
                y[0]*t1*t1*t1
                +
                y[1]*t*t1*t1*3
                +
                y[2]*t*t*t1*3
                +
                y[3]*t*t*t;
        }

        dxf_polyline(file, N, xb, yb);

        x += 3;
        y += 3;
    }
}


void
dxf_circle
(
    FILE	*file,
    double	x1,
    double	y1,
    double	rad
)
{
    fprintf(file, "0\nCIRCLE\n");
    fprintf(file, "8\n%d\n", dxf_layer);
    fprintf(file, "10\n%lf\n", x1);
    fprintf(file, "20\n%lf\n", y1);
    fprintf(file, "40\n%lf\n", rad);
}


void
dxf_arc
(
    FILE	*file,
    double	x1,
    double	y1,
    double	x2,
    double	y2,
    double	xstart,
    double	ystart,
    double	xend,
    double	yend
)
{
    double      xc = (x1 + x2) / 2;
    double      yc = (y1 + y2) / 2;
    double      a1 = 90 - atan2(xstart - xc, ystart - yc) * 180/PI;
    double      a2 = 90 - atan2(xend - xc, yend - yc) * 180/PI;

    fprintf(file, "0\nARC\n");
    fprintf(file, "8\n%d\n", dxf_layer);
    fprintf(file, "10\n%lf\n", xc);
    fprintf(file, "20\n%lf\n", yc);
    fprintf(file, "40\n%lf\n", (x2 - x1) / 2);  // assume it's circular
    fprintf(file, "50\n%lf\n", a1);
    fprintf(file, "51\n%lf\n", a2);
}

