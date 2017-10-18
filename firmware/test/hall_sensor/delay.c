#include "delay.h"
#include "stm32f4xx_it.h"

void delay(uint32_t ms)
{
	uint64_t hold_time = get_time();
	while (get_time() - hold_time <= ms)
		;
}
