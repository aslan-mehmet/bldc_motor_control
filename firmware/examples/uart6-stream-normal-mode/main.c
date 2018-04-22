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
#include "ang_spd_sensor.h"
#include "serial_packet_sent_cmd_ids.h"
#define temp_arbitrary_gpio_pin_on() GPIOC->ODR |= GPIO_ODR_ODR_9
#define temp_arbitrary_gpio_pin_off() GPIOC->ODR &= ~GPIO_ODR_ODR_9
#define temp_arbitrary_gpio_pin_toggle() GPIOC->ODR ^= GPIO_ODR_ODR_9

uint8_t _dma_transfer_done_flag = 0;
uint8_t _adc_bemfs_readings[4];

void temp_arbitrary_gpio_pin_init(void)
{
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
        /* ihm07 cn10 pin 1 */
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
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
        if (serial_packet_init()) {
                while (1)
                        ;
        }
        ihm07_led_red_init();
        ihm07_l6230_pins_init();
        uint64_t hold_time = get_time();
        /* write after this line */

        ihm07_analog_pins_init();
        uint8_t adc_bemf_chs[3] = {IHM07_ADC_CH_BEMF1, IHM07_ADC_CH_BEMF2, IHM07_ADC_CH_BEMF3};
        ihm07_adc_dma_group_mode_init(adc_bemf_chs, _adc_bemfs_readings + 1, 3);
        ihm07_adc_dma_interrupt_init();
        ihm07_adc_dma_interrupt_connection_state(ENABLE);
        ihm07_adc_dma_state(ENABLE);
        ihm07_adc_state(ENABLE);

        six_step_hall_init();
        six_step_hall_set_pwm_val(500);
        six_step_hall_start();

        /* TODO: six_step_hall_init if not called, pwm timer clk not enabled */
        /* FIX IT LATER */
        ihm07_pwm_duty_interrupt_init();
        ihm07_pwm_duty_interrupt_connection_state(ENABLE);
        ihm07_pwm_duty_set_val(1);

        _adc_bemfs_readings[0] = 0xaa;

        temp_arbitrary_gpio_pin_init();
        uart6_stream_single_periph_init();

        while (1) {
                if (ang_spd_sensor_exist_new_value()) {
                        float f = ang_spd_sensor_get_in_rpm();
                        serial_packet_encode_poll(PRINT_SPD_RPM, sizeof(float), &f);
                }

                if (_dma_transfer_done_flag == 5) {
                        _dma_transfer_done_flag = 0;
                        temp_arbitrary_gpio_pin_on();

                        uart6_stream_single_config_dma(_adc_bemfs_readings, 4);
                        uart6_stream_start();

                        temp_arbitrary_gpio_pin_off();
                }

                /* dont touch this lines */
                if (get_time() - hold_time > 100) {
                        hold_time = get_time();
                        ihm07_led_red_toggle();
                }
                serial_packet_flush();
        }
}

void ihm07_pwm_duty_interrupt_callback(void)
{
        //temp_arbitrary_gpio_pin_on();
        ihm07_adc_start_conversion();
}

void ihm07_adc_dma_transfer_complete_callback(void)
{
        _dma_transfer_done_flag++;

        if (_dma_transfer_done_flag == 20) {
                _dma_transfer_done_flag = 0;
        }
        //temp_arbitrary_gpio_pin_off();
}

void serial_packet_print(uint8_t byt)
{
        uart_send_byte_poll(byt);
}
