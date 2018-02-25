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
        ihm07_l6230_pins_init();

        ihm07_analog_pins_init();
        uint8_t adc_channels[3] = {IHM07_ADC_CH_BEMF1, IHM07_ADC_CH_BEMF2, IHM07_ADC_CH_BEMF3};
        ihm07_adc_group_mode_init(adc_channels, 3);
        ihm07_adc_state(ENABLE);
        uint16_t buffer[4];
        buffer[0] = 0xaabb;

        uint64_t hold_time = get_time();

        while (1) {
                serial_packet_flush();

                for (int i = 0; i < 3; ++i) {
                        ihm07_adc_start_conversion();
                        ihm07_adc_wait_conversion();
                        buffer[i+1] = ihm07_adc_get_conversion_val();
                }

                uart_send_buffer_poll((uint8_t *) buffer, sizeof(uint16_t) * 4);

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
