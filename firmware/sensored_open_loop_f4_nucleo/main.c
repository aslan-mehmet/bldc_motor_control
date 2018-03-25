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

uint8_t _dma_transfer_done_flag = 0;

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
        uint64_t hold_time = get_time();
        /* write after this line */

        ihm07_analog_pins_init();
        uint8_t adc_ch_pot = IHM07_ADC_CH_POT;
        uint8_t pot_val = 0;
        ihm07_adc_dma_group_mode_init(&adc_ch_pot, &pot_val, 1);
        ihm07_adc_dma_interrupt_init();
        ihm07_adc_dma_interrupt_connection_state(ENABLE);
        ihm07_adc_dma_state(ENABLE);
        ihm07_adc_state(ENABLE);

        ihm07_adc_start_conversion();

        while (1) {
                if (_dma_transfer_done_flag) {
                        _dma_transfer_done_flag = 0;
                        uart_send_byte_poll(pot_val);
                        ihm07_adc_start_conversion();
                }

                /* dont touch this lines */
                if (get_time() - hold_time > 100) {
                        hold_time = get_time();
                        ihm07_led_red_toggle();
                }
                serial_packet_flush();
        }
}

void ihm07_adc_dma_transfer_complete_callback(void)
{
        _dma_transfer_done_flag = 1;
}

void serial_packet_print(uint8_t byt)
{
        uart_send_byte_poll(byt);
}
