#include <stdlib.h>
#include <slope/slope.h>
#include "plotter.h"

SlopeScale *_scale;
SlopeItem *_series;
GtkWidget *_chart;

/* if NULL plotter not created or destroyed */
static double *_x = NULL, *_y = NULL;
static int _n_pts = 0;
static double _y_range[2] = {0, 1};

static void plotter_close(GtkWidget *widget, gpointer data)
{
        if (_x == NULL) {
                return;
        }
        plotter_destroy();
        puts("chart closed");
}

/* create plotter, allocate memory */
int plotter_create(int mem)
{
        /* so alread created */
        if (_x != NULL) {
                plotter_destroy();
        }
        
        _x = (double *) malloc(sizeof(double) * mem);
        _y = (double *) malloc(sizeof(double) * mem);
        /* default show all points in memory */
        _n_pts = mem;
        
        if (_x == NULL || _y == NULL) {
                if (_x != NULL) {
                        free(_x);
                }

                if (_y != NULL) {
                        free(_y);
                }

                return -1;
        }

        for (int i = 0; i < _n_pts; ++i) {
                _x[i] = 0;
                _y[i] = 0;
        }
        _x[_n_pts-1] = 1;

        /* _chart is a widget */
        _chart = slope_chart_new();        

        g_signal_connect(G_OBJECT(_chart), "destroy", G_CALLBACK(plotter_close), NULL);


        /* DONT HAVE CLUE HOW GObject WORKS */
        /* return SLOPE_SCALE(self) */
        /* #define SLOPE_SCALE(obj) \ */
        /* (G_TYPE_CHECK_INSTANCE_CAST((obj), SLOPE_SCALE_TYPE, SlopeScale)) */
        /* #define SLOPE_SCALE_TYPE (slope_scale_get_type()) */
        _scale = slope_xyscale_new();
        slope_chart_add_scale(SLOPE_CHART(_chart), _scale);
        _series = slope_xyseries_new_filled("data", _x, _y, _n_pts, "b-");
        slope_scale_add_item(_scale, _series);
        slope_xyscale_set_x_range(SLOPE_XYSCALE(_scale), _x[0], _x[_n_pts - 1]);
        slope_xyscale_set_y_range(SLOPE_XYSCALE(_scale), _y_range[0], _y_range[1]);

        gtk_widget_show_all(_chart);
}

/* no longer needed, delete from existence*/
void plotter_destroy(void)
{
        if (_x != NULL) {
                free(_x);
                _x = NULL;
        }

        if (_y != NULL) {
                free(_y);
                _y = NULL;
        }

        /* SlopeScale *_scale; */
        /* SlopeItem *_series; */
        /* GtkWidget *_chart; */
}


void default_plotter(void)
{
        gtk_init(NULL, NULL);        
}
/* user wants out */
void exit_plotter(void)
{
        if (_x == NULL) {
                return;
        }
                
        plotter_destroy();
}

/* number of points to display before last received data */
void plotter_set_n_points(int n_pts)
{
        if (_x == NULL) {
                return;
        }
        
        plotter_destroy();
        plotter_create(n_pts);
}

void plotter_set_yrange(double min, double max)
{
        if (_x == NULL) {
                return;
        }
        
        _y_range[0] = min;
        _y_range[1] = max;
}

static void plotter_redraw(void)
{
        if (_x == NULL) {
                return;
        }
        
	slope_xyseries_update_data(SLOPE_XYSERIES(_series), _x, _y, _n_pts);
	slope_xyscale_set_x_range(SLOPE_XYSCALE(_scale), _x[0], _x[_n_pts - 1]);
	slope_xyscale_set_y_range(SLOPE_XYSCALE(_scale), _y_range[0], _y_range[1]);
	slope_chart_redraw(SLOPE_CHART(_chart));
}

/* put incoming data to be plotted */
void plotter_put_data(double data)
{
        if (_x == NULL) {
                return;
        }

        int last = _n_pts - 1;
        
        for (int i = 0; i < last; ++i) {
                _x[i] = _x[i+1];
                _y[i] = _y[i+1];
        }

        _x[last]++;
        _y[last] = data;
        plotter_redraw();
}

/* redraw if new data received */
void plotter_loop(void)
{
        if (_x == NULL) {
                return;
        }
        /* single iteration of the main loop */
        /* if event not avaliable get out */
        gtk_main_iteration_do(FALSE);
}
