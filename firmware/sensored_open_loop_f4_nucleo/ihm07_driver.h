#ifndef __IHM07_DRIVER_H
#define __IHM07_DRIVER_H

#include "stm32f4xx.h"
#include "ihm07_driver_pins.h"

void ihm07_hall_pins_init(void);
void ihm07_hall_interrupt_init(void);
/* void ihm07_hall_state_change_callback(void) __attribute__ ((weak)) */
void ihm07_hall_interrupt_connection_state(FunctionalState state);
uint8_t ihm07_hall_read(void);

void ihm07_led_red_init(void);
#define ihm07_led_red_on() PORT_LED_RED->ODR |= PIN_LED_RED
#define ihm07_led_red_off() PORT_LED_RED->ODR &= ~PIN_LED_RED
#define ihm07_led_red_toggle() PORT_LED_RED->ODR ^= PIN_LED_RED
/* single inc 23.81ns */
#define PWM_MAX_VAL ((uint16_t) 1e3)
/* pwm freq 42kHz, freq 23.81us */
void ihm07_pwm_and_pins_init(void);
#define ihm07_pwm_set_ch1(val) TIM1->CCR1 = (val) > PWM_MAX_VAL ? PWM_MAX_VAL : (val)
#define ihm07_pwm_set_ch2(val) TIM1->CCR2 = (val) > PWM_MAX_VAL ? PWM_MAX_VAL : (val)
#define ihm07_pwm_set_ch3(val) TIM1->CCR3 = (val) > PWM_MAX_VAL ? PWM_MAX_VAL : (val)

void ihm07_pwm_duty_interrupt_init(void);
/* void ihm07_pwm_duty_interrupt_callback(void) */
void ihm07_pwm_duty_interrupt_connection_state(FunctionalState state);

/* put them also reset state */
void ihm07_l6230_pins_init(void);

#define ihm07_l6230_enable() PORT_DIAG_EN->ODR |= PIN_DIAG_EN
#define ihm07_l6230_disable() PORT_DIAG_EN->ODR &= ~PIN_DIAG_EN

#define ihm07_l6230_ch1_enable() PORT_EN1_EN2_EN3->ODR |= PIN_EN1
#define ihm07_l6230_ch2_enable() PORT_EN1_EN2_EN3->ODR |= PIN_EN2
#define ihm07_l6230_ch3_enable() PORT_EN1_EN2_EN3->ODR |= PIN_EN3

#define ihm07_l6230_ch1_disable() PORT_EN1_EN2_EN3->ODR &= ~PIN_EN1
#define ihm07_l6230_ch2_disable() PORT_EN1_EN2_EN3->ODR &= ~PIN_EN2
#define ihm07_l6230_ch3_disable() PORT_EN1_EN2_EN3->ODR &= ~PIN_EN3
#define ihm07_l6230_ch_all_disable() PORT_EN1_EN2_EN3->ODR &= ~(PIN_EN1 | PIN_EN2 | PIN_EN3)

void ihm07_analog_pins_init(void);
void ihm07_adc_single_mode_init(uint8_t IHM07_ADC_CH_x);
/* start conversion scans all given channels in group */
void ihm07_adc_group_mode_init(uint8_t *IHM07_ADC_CH_x, uint8_t number_of_channels);
#define ihm07_adc_state(state) ADC_Cmd(ADC1, (state))

#define ihm07_adc_get_conversion_val() ADC_GetConversionValue(ADC1)
#define ihm07_adc_start_conversion() ADC_SoftwareStartConv(ADC1)
#define ihm07_adc_wait_conversion() while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
uint16_t ihm07_adc_single_read_channel(uint8_t IHM07_ADC_CH_x);

#define ihm07_adc_interrupt_init() ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE)
/* void ihm07_adc_eoc_callback(void) */
void ihm07_adc_interrupt_connection_state(FunctionalState state);


#endif /* __IHM07_DRIVER_H */
