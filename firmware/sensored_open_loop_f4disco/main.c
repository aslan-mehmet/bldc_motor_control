/*
 * author: Mehmet ASLAN
 * date: November 1, 2017
 * no warranty, no licence agreement
 * use it at your own risk
 */
#include "main.h"

int main(void)
{
	if (SysTick_Config(SystemCoreClock / 1000)) {
		while (1)
			;
	}

	board_led_init();
	motor_init();
	motor_start();
	motor_set_pwm(500);

	hall_sensor_init();

	uart_init();

	while (1)
		;
	
	return 0;
}
