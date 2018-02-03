#include <math.h>
#include <slope/slope.h>
#include <stdio.h>

SlopeScale *scale;
SlopeItem *series;
GtkWidget *chart;
const int len = 500;
const double delta_angle = 4.0 * G_PI / 500;
double *angle, *mag;

void init(void)
{
	for (int i = 0; i < len; ++i) {
		angle[i] = delta_angle * i;
		mag[i] = 0;
	}
}

void push(void)
{
	static long int count = 0;

	if (count < len) {
		mag[count] = sin(angle[count]);
//		printf("angle: %f mag: %f\n", angle[count], mag[count]);
	} else {
		for (int i = 0; i < len - 1; ++i) {
			angle[i] = angle[i+1];
			mag[i] = mag[i+1];
		}

		angle[len-1] = angle[len-2] + delta_angle;
		mag[len-1] = sin(angle[len-1]);
//		printf("angle: %f mag: %f\n", angle[len-1], mag[len-1]);
	}

	count++;
}



static gboolean timer_callback(GtkWidget *chart) {
	push();
	slope_xyseries_update_data(SLOPE_XYSERIES(series), angle, mag, len);
	slope_xyscale_set_x_range(SLOPE_XYSCALE(scale), angle[0], angle[len-1]);
	slope_xyscale_set_y_range(SLOPE_XYSCALE(scale), -1, 1);
	slope_chart_redraw(SLOPE_CHART(chart));
	return TRUE;
}

int main(int argc, char *argv[]) {

	gtk_init(&argc, &argv);
	chart = slope_chart_new();
	g_signal_connect(G_OBJECT(chart), "destroy", G_CALLBACK(gtk_main_quit), NULL);

	/* create some sinusoidal data points */
	angle = g_malloc(len * sizeof(double));
	mag = g_malloc(len * sizeof(double));

	init();

	scale = slope_xyscale_new();
	slope_chart_add_scale(SLOPE_CHART(chart), scale);

	series = slope_xyseries_new_filled("Wave", angle, mag, len, "b-");
	slope_scale_add_item(scale, series);
	slope_xyscale_set_x_range(SLOPE_XYSCALE(scale), angle[0], angle[len-1]);
	slope_xyscale_set_y_range(SLOPE_XYSCALE(scale), -1, 1);

	g_timeout_add(30, (GSourceFunc) timer_callback, (gpointer) chart);
	gtk_widget_show_all(chart);
	gtk_main();

	g_free(angle);
	g_free(mag);
	return 0;
}




