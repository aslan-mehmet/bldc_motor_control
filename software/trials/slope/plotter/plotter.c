#include <slope/slope.h>

double *_x = NULL, *_y = NULL;
/* number of points to draw */
/* y_len = x_len */
int _x_len;
/* y axis borders */
double _y_min, _y_max;
int _x_index = 0;

SlopeScale *scale;
SlopeItem *series;
GtkWidget *chart;

static void plotter_create_figure(void)
{
	gtk_init(NULL, NULL);
	chart = slope_chart_new();
	g_signal_connect(G_OBJECT(chart), "destroy", G_CALLBACK(gtk_main_quit), NULL);

	scale = slope_xyscale_new();
	slope_chart_add_scale(SLOPE_CHART(chart), scale);

	series = slope_xyseries_new_filled("Data", _x, _y, _x_len, "b-");
	slope_scale_add_item(scale, series);
	slope_xyscale_set_x_range(SLOPE_XYSCALE(scale), _x[0], _x[_x_len - 1]);
	slope_xyscale_set_y_range(SLOPE_XYSCALE(scale), _y_min, _y_max);

	gtk_widget_show_all(chart);
}

int plotter_create(int x_len,
                  double y_min,
                  double y_max)
{
	_y_min = y_min;
	_y_max = y_max;
	_x_len = x_len;

	_x = (double *) g_malloc(sizeof(double) * x_len);
	_y = (double *) g_malloc(sizeof(double) * x_len);

	if (_x == NULL | _y == NULL) {
		return -1;
	}

	for (int i = 0; i < _x_len; i++) {
		_x[i] = i;
		_y[i] = 0;
	}

	plotter_create_figure();

	return 0;
}



void plotter_destroy(void)
{
	if (_x == NULL) {
		return;
	}

	g_free(_x);
	g_free(_y);

	_x = NULL;
	_y = NULL;
}

static void plotter_redraw(void)
{
	slope_xyseries_update_data(SLOPE_XYSERIES(series), _x, _y, _x_len);
	slope_xyscale_set_x_range(SLOPE_XYSCALE(scale), _x[0], _x[_x_len - 1]);
	slope_xyscale_set_y_range(SLOPE_XYSCALE(scale), _y_min, _y_max);
	slope_chart_redraw(SLOPE_CHART(chart));
}

void plotter_put_data(double data)
{
	if (_x == NULL) {
		return;
	}

	if (_x_index < _x_len) {
		_y[_x_index++] = data;
	} else  {
		int last = _x_len - 1;

		for (int i = 0; i < last; i++) {
			_x[i] = _x[i+1];
			_y[i] = _y[i+1];
		}

		_x[last] = _x_index;
		_y[last] = data;

		_x_index++;
	}
	plotter_redraw();
}

void plotter_loop(void)
{
	gtk_main_iteration_do(FALSE);
}

void exit_plotter(void)
{
	plotter_destroy();
}
