#include <stdio.h>
#include <time.h>

int delay_ms(void)
{
	struct timespec t;

	t.tv_sec = 0;
	/* max 1 sec */
	t.tv_nsec = 1e+6;

	return (int) nanosleep(&t, NULL);
}

int main(void)
{

	if (! delay_ms()) {
		puts("suc");
	}

	while (1) {

		for (int i = 0; i < 1e+3; ++i) {
			delay_ms();
		}

		puts("one sec wasted");
	}

	return 0;
}
