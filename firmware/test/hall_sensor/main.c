/*
 * author: Mehmet ASLAN
 * date: October 17, 2017
 * no warranty, no licence agreement
 * use it at your own risk
 */
#include "stm32f4xx.h"
void hall_sensor_init(void);
void uart_init(void);

/*
 * sensor have six steps
 * cw step order : 2 3 1 5 4 6
 */
uint8_t hall_current_step = 0;

/* read motor hall sensor print to uart */
void main(void)
{
	uart_init();
	hall_sensor_init();

	while (1)
		;
}

void uart_init(void)
{
	
}

void hall_sensor_init(void)
{
	/* setup 3 pin change interrupts */
	/* 5v tolerant */
}

/* isr : read pins, print to uart */

/* 
 * uart receive interrupt 
 * needed later
 */
