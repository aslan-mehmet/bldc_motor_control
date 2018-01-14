#ifndef __PLOTTER_H
#define __PLOTTER_H

/* create plotter, allocate memory */
int plotter_create(int mem);
/* no longer needed, delete from existence*/
void plotter_destroy(void);
/* user wants out */
void exit_plotter(void);

/* number of points to display before last received data */
void plotter_show_n_points(int n_pts);
void plotter_set_yrange(double min, double max);

/* put incoming data to be plotted */
void plotter_put_data(double data);
/* redraw if new data received */
void plotter_loop(void);
void default_plotter(void);
#endif
