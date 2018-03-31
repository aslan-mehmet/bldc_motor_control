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

static uint16_t _pwm_val = 0;

static void six_step_commutate_ccw(uint8_t state)
{
        ihm07_pwm_set_ch1(0);
        ihm07_pwm_set_ch2(0);
        ihm07_pwm_set_ch3(0);
        ihm07_l6230_ch_all_disable();

        switch (state) {
        case 5:
                ihm07_l6230_ch1_enable();
                ihm07_l6230_ch2_enable();
                ihm07_pwm_set_ch2(_pwm_val);
                break;
        case 4:
                ihm07_l6230_ch1_enable();
                ihm07_l6230_ch3_enable();
                ihm07_pwm_set_ch3(_pwm_val);
                break;
        case 6:
                ihm07_l6230_ch2_enable();
                ihm07_l6230_ch3_enable();
                ihm07_pwm_set_ch3(_pwm_val);
                break;
        case 2:
                ihm07_l6230_ch1_enable();
                ihm07_l6230_ch2_enable();
                ihm07_pwm_set_ch1(_pwm_val);
                break;
        case 3:
                ihm07_l6230_ch1_enable();
                ihm07_l6230_ch3_enable();
                ihm07_pwm_set_ch1(_pwm_val);
                break;
        case 1:
                ihm07_l6230_ch2_enable();
                ihm07_l6230_ch3_enable();
                ihm07_pwm_set_ch2(_pwm_val);
                break;
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
        if (serial_packet_init()) {
                while (1)
                        ;
        }
        ihm07_led_red_init();
        ihm07_l6230_pins_init();
        uint64_t hold_time = get_time();
        /* write after this line */

        ihm07_pwm_and_pins_init();
        ihm07_l6230_enable();
        ihm07_pwm_state(ENABLE);
        ihm07_l6230_ch_all_disable();

        ihm07_analog_pins_init();
        ihm07_adc_single_mode_init(IHM07_ADC_CH_POT);
        ihm07_adc_state(ENABLE);

        uint8_t six_step_lut[6] = {5, 4, 6, 2, 3, 1};
        uint64_t hold_time_for_step = get_time();
        uint8_t six_step_index = 0;

        uint16_t pot_val[3];
        uint16_t pot_val_avg;

        while (1) {
                pot_val[0] = ihm07_adc_single_read_channel(IHM07_ADC_CH_POT);
                pot_val[1] = ihm07_adc_single_read_channel(IHM07_ADC_CH_POT);
                pot_val[2] = ihm07_adc_single_read_channel(IHM07_ADC_CH_POT);
                pot_val_avg = (uint16_t) ((pot_val[0] + pot_val[1] + pot_val[2]) / 3.0);
                _pwm_val = (uint16_t) (pot_val_avg * 1000 / 4095.0);
                serial_packet_encode_poll(PRINT_PWM_VAL, sizeof(uint16_t), &_pwm_val);

                if (get_time() - hold_time_for_step > 10) {
                        hold_time_for_step = get_time();

                        six_step_commutate_ccw(six_step_lut[six_step_index++]);

                        if (six_step_index > 5) {
                                six_step_index = 0;
                        }
                }

                /* dont touch this lines */
                if (get_time() - hold_time > 100) {
                        hold_time = get_time();
                        ihm07_led_red_toggle();
                }
                serial_packet_flush();
        }
}

/* dont touch this lines */
void serial_packet_print(uint8_t byt)
{
        uart_send_byte_poll(byt);
}
