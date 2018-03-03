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

uint16_t _adc_buffer[4];
uint8_t _adc_iter = 0;

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
        ihm07_l6230_pins_init();

        six_step_hall_init();
        six_step_hall_set_direction(SIX_STEP_HALL_DIRECTION_CCW);
        //six_step_hall_start();

        ihm07_analog_pins_init();
        ihm07_adc_single_mode_init(IHM07_ADC_CH_BEMF1);
        ihm07_adc_state(ENABLE);

        uint64_t hold_time = get_time();
        uint16_t bemfs[3];
        uint64_t time;

        uint8_t buffer[sizeof(bemfs) + sizeof(time)];
        #define PRINT_BEMFS 0
        while (1) {
                serial_packet_flush();

                time = get_time();
                bemfs[0] = ihm07_adc_single_read_channel(IHM07_ADC_CH_BEMF1);
                bemfs[1] = ihm07_adc_single_read_channel(IHM07_ADC_CH_BEMF2);
                bemfs[2] = ihm07_adc_single_read_channel(IHM07_ADC_CH_BEMF3);

                safe_memory_copy(buffer, &time, sizeof(time));
                safe_memory_copy(buffer+sizeof(time), bemfs, sizeof(bemfs));

                serial_packet_encode_poll(PRINT_BEMFS, sizeof(time) + sizeof(bemfs), buffer);

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
