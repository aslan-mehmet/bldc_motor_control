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

        ihm07_analog_pins_init();
        uint8_t adc_channels[4] = {IHM07_ADC_CH_BEMF1, IHM07_ADC_CH_BEMF2,
                                        IHM07_ADC_CH_BEMF3, IHM07_ADC_CH_POT};
        ihm07_adc_group_mode_init(adc_channels, 4);
        ihm07_adc_interrupt_init();
        ihm07_adc_interrupt_connection_state(ENABLE);
        ihm07_adc_state(ENABLE);

        uint64_t hold_time = get_time();

        while (1) {
                serial_packet_flush();

                uart_send_byte_poll(0xbb);
                uart_send_byte_poll(0xaa);
                uart_send_buffer_poll((uint8_t *) _adc_buffer, sizeof(uint16_t) * 4);

                if (get_time() - hold_time > 100) {
                        hold_time = get_time();
                        ihm07_led_red_toggle();
                        ihm07_adc_start_conversion();
                }
        }
}

void ihm07_adc_eoc_callback(void)
{
        _adc_buffer[_adc_iter++] = ihm07_adc_get_conversion_val();

        if (_adc_iter == 4) {
                _adc_iter = 0;
        }
}

void serial_packet_print(uint8_t byt)
{
        uart_send_byte_poll(byt);
}
