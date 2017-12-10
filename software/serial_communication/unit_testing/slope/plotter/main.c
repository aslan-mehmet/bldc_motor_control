#include <stdio.h>
#include <unistd.h>
#include "plotter.h"
#include <time.h>

static void delay_ms(int ms)
{
	struct timespec t;

	t.tv_sec = 0;
	/* max 1 sec */
	t.tv_nsec = 1e+6;

	for (int i = 0; i < ms; ++i) {
		nanosleep(&t, NULL);
	}
}

int main(void)
{
	int iter = 0;
	if (plotter_create(1000, 0, 1000)) {
		puts("fail\n");
		return 0;
	}

	while (1) {
		printf("%d\n", iter++);

		plotter_put_data(iter);
		plotter_loop();

		delay_ms(10);

		if (iter == 1000) {
			iter = 0;
		}
	}
	return 0;
}
