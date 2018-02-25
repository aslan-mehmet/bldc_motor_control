/*
 * author: Mehmet ASLAN
 * date: January 24, 2018
 * no warranty, no licence agreement
 * use it at your own risk
 */
#include "stm32f4xx.h"
#include "time.h"
#include "ihm07_driver.h"
#include "serial_packet.h"
#include "uart.h"
#include "six_step_hall.h"

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
        if (serial_packet_init()) {
                while (1)
                        ;
        }

        ihm07_led_red_init();
        six_step_hall_init();
        six_step_hall_set_direction(SIX_STEP_HALL_DIRECTION_CCW);
        six_step_hall_set_pwm_val(500);
        six_step_hall_start();

        uint64_t hold_time = get_time();

        while (1) {
                serial_packet_flush();

                if (get_time() - hold_time > 100) {
                        hold_time = get_time();
                        ihm07_led_red_toggle();
                }
        }
}

void serial_packet_print(uint8_t byt)
{
        uart_send_byte_poll(byt);
}
