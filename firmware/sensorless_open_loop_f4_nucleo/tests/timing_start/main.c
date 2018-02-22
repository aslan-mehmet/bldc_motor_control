/*
 * author: Mehmet ASLAN
 * date: January 24, 2018
 * no warranty, no licence agreement
 * use it at your own risk
 */
#include "stm32f4xx.h"
#include "delay.h"
#include "uart.h"
#include "motor.h"

extern void motor_commutate(uint8_t step_number);
int pwm = 220;
void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		uint16_t data = USART_ReceiveData(USART1);
                pwm = data;
	}
}

int main(void)
{
        /* 1ms tick */
	if (SysTick_Config(SystemCoreClock / 1000)) {
		while (1)
			;
	}

        /* @arg NVIC_PriorityGroup_2: 2 bits for pre-emption priority */
        /*                            2 bits for subpriority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	/* syscfg reset, so only change interrupt you use*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	uart_init();
        motor_init();
        motor_set_pwm(pwm);
        motor_start();

        uint8_t arr[] = {'s', 't', 'a', 'r', 't', '\n'};
        uart_send_poll_bfr(arr, 6);

        uint8_t step_seq[6] = {5, 4, 6, 2, 3, 1};

	while (1) {
                for (int i = 0; i < 6; ++i) {
                        motor_commutate(step_seq[i]);
                        delay(10);
                }
                motor_set_pwm(pwm);
        }
}
