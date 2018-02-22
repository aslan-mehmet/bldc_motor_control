#ifndef __IHM07_DRIVER_H
#define __IHM07_DRIVER_H

#include "stm32f4xx.h"
#include "ihm07_driver_pins.h"

void ihm07_hall_pins_init(void);
/* void ihm07_hall_state_change_callback(void) */
void ihm07_hall_interrupt_init(void);
void ihm07_hall_interrupt_set_state(FunctionalState state);
uint8_t ihm07_hall_read(void);

/* GPIO_BEMF DIAG_EN put them in their reset state*/
void ihm07_board_and_pins_init(void);
#define ihm07_l6230_enable() PORT_DIAG_EN->ODR |= PIN_DIAG_ODR
#define ihm07_l6230_disable() PORT_DIAG_EN->ODR &= ~PIN_DIAG_ODR

/* sets up pins and timer */
void ihm07_pwm_and_pins_init(void);
#define ihm07_l6230_enable_ch1() PORT_EN->ODR |= PIN_EN1_ODR
#define ihm07_l6230_enable_ch2() PORT_EN->ODR |= PIN_EN2_ODR
#define ihm07_l6230_enable_ch3() PORT_EN->ODR |= PIN_EN3_ODR

#define ihm07_l6230_disable_ch1() PORT_EN->ODR &= ~PIN_EN1_ODR
#define ihm07_l6230_disable_ch2() PORT_EN->ODR &= ~PIN_EN2_ODR
#define ihm07_l6230_disable_ch3() PORT_EN->ODR &= ~PIN_EN3_ODR

#define PWM_MAX ((uint16_t) 1e3)
#define ihm07_pwm_set_ch1(val) TIM1->CCR1 = (val) > PWM_MAX ? PWM_MAX : (val)
#define ihm07_pwm_set_ch2(val) TIM1->CCR2 = (val) > PWM_MAX ? PWM_MAX : (val)
#define ihm07_pwm_set_ch3(val) TIM1->CCR3 = (val) > PWM_MAX ? PWM_MAX : (val)

void ihm07_led_red_init(void);
#define ihm07_led_red_on() PORT_LED_RED |= PIN_LED_RED_ODR
#define ihm07_led_red_off() PORT_LED_RED &= ~PIN_LED_RED_ODR
#define ihm07_led_red_toggle() PORT_LED_RED ^= PIN_LED_RED_ODR

#define IHM07_ADC_CH_BEMF1 ADC_Channel_13
#define IHM07_ADC_CH_BEMF2 ADC_Channel_8
#define IHM07_ADC_CH_BEMF3 ADC_Channel_7
#define IHM07_ADC_CH_POT ADC_Channel_9
/* set all pins to analog and adc */
void ihm07_adc_init(void);
uint16_t ihm07_adc_read_single_channel(uint8_t channel);






#endif // __IHM07_DRIVER_H
