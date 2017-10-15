#include "f4board.h"
#include "stm32f4xx.h"

void board_led_init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitTypeDef g;
                                                                      
	g.GPIO_Pin = LED_GREEN | LED_ORANGE | LED_RED | LED_BLUE;
	g.GPIO_Mode = GPIO_Mode_OUT;                       
	g.GPIO_OType = GPIO_OType_PP;                      
	g.GPIO_Speed = GPIO_Speed_100MHz;                  
	g.GPIO_PuPd = GPIO_PuPd_NOPULL;                    
	GPIO_Init(GPIOD, &g);
}
