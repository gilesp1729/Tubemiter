FILE *
dxf_open
(
    char        *filename
);

void
dxf_close
(
    FILE	*file
);

void
dxf_set_layer
(
    int         layer
);

void
dxf_line
(
    FILE	*file,
    double	x1,
    double	y1,
    double	x2,
    double	y2
);

void
dxf_polyline
(
    FILE	*file,
    int	        n_pts,
    double	*x,
    double	*y
);

void
dxf_polybezier
(
    FILE	*file,
    int	        n_pts,
    double	*x,
    double	*y
);

void
dxf_circle
(
    FILE	*file,
    double	x1,
    double	y1,
    double	rad
);

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
);
