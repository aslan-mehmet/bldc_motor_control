#ifndef __PLOTTER_H
#define __PLOTTER_H

int plotter_create(int x_len, double y_min, double y_max);
void plotter_destroy(void);
void plotter_put_data(double data);
void plotter_loop(void);
void exit_plotter(void);

#endif
