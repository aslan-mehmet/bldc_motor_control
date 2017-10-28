/*
 * author: Mehmet ASLAN
 * date: October 28, 2017
 * no warranty, no licence agreement
 * use it at your own risk
 */
#include "main.h"

int main(void)
{
	/* 1ms tick */
	if (SysTick_Config(SystemCoreClock / 1000)) {
		while (1)
			;
	}
	
	board_led_init();
	hall_init();
	motor_init();
	
	while (1)
		;
	
	return 0;
}
