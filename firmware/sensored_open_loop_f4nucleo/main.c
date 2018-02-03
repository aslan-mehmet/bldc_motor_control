/*
 * author: Mehmet ASLAN
 * date: January 24, 2018
 * no warranty, no licence agreement
 * use it at your own risk
 */
#include "stm32f4xx.h"
#include "delay.h"
#include "f4nucleo_board.h"
#include "bits.h"
#include "morse.h"

int main(void)
{
        RCC_ClocksTypeDef r;
        RCC_GetClocksFreq(&r);

        /* 1ms tick */
	if (SysTick_Config(SystemCoreClock / 1000)) {
		while (1)
			;
	}

        /* @arg NVIC_PriorityGroup_2: 2 bits for pre-emption priority */
        /*                            2 bits for subpriority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	board_led_init();

        char arr[2]={'\0', '\0'};
        for (int i = 'a'; i <= 'z'; ++i) {
                arr[0] = i;
                buz_word(arr);
        }

        for (int i = '0'; i <= '9'; ++i) {
                arr[0] = i;
                buz_word(arr);
        }
}
