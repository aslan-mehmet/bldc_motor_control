/*
 * author: Mehmet ASLAN
 * date: January 24, 2018
 * no warranty, no licence agreement
 * use it at your own risk
 */
#include "stm32f4xx.h"
#include "time.h"
#include "ihm07_driver.h"
#include "serial_driver.h"

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

        serial_driver_init();
        ihm07_led_red_init();

        ihm07_l6230_pins_init();
        ihm07_pwm_and_pins_init();
        ihm07_pwm_duty_interrupt_init();
        ihm07_pwm_duty_interrupt_connection_state(ENABLE);

        while (1) {
                delay(100);
                ihm07_led_red_toggle();
        }
}

void ihm07_pwm_duty_interrupt_callback(void)
{
        PORT_EN1_EN2_EN3->ODR ^= PIN_EN1;
}
