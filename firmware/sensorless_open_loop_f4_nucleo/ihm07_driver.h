#ifndef __IHM07_DRIVER_H
#define __IHM07_DRIVER_H

#include "stm32f4xx.h"

void ihm07_hall_pins_init(void);
/* void ihm07_hall_state_change_callback(void) */
void ihm07_hall_interrupt_init(void);
void ihm07_hall_interrupt_set_state(FunctionalState state);
uint8_t ihm07_hall_read(void);

/* gpio config done in ihm07_pwm_init */
#define ihm07_l6230_enable() GPIOA->BSRRL = GPIO_BSRR_BS_5
#define ihm07_l6230_disable() GPIOA->BSRRH = GPIO_BSRR_BR_5

/* sets up pins and timer */
void ihm07_pwm_init(void);
#define ihm07_l6230_enable_ch1() GPIOC->BSRRL = GPIO_BSRR_BS_10
#define ihm07_l6230_enable_ch2() GPIOC->BSRRL = GPIO_BSRR_BS_11
#define ihm07_l6230_enable_ch3() GPIOC->BSRRL = GPIO_BSRR_BS_12

#define ihm07_l6230_disable_ch1() GPIOC->BSRRH = GPIO_BSRR_BR_10
#define ihm07_l6230_disable_ch2() GPIOC->BSRRH = GPIO_BSRR_BR_11
#define ihm07_l6230_disable_ch3() GPIOC->BSRRH = GPIO_BSRR_BR_12

#define PWM_MAX ((uint16_t) 1e3)
#define ihm07_pwm_set_ch1(val) TIM1->CCR1 = (val) > PWM_MAX ? PWM_MAX : (val)
#define ihm07_pwm_set_ch2(val) TIM1->CCR2 = (val) > PWM_MAX ? PWM_MAX : (val)
#define ihm07_pwm_set_ch3(val) TIM1->CCR3 = (val) > PWM_MAX ? PWM_MAX : (val)

void ihm07_led_red_init(void);
#define ihm07_led_red_on() GPIOB->BSRRL = GPIO_BSRR_BS_2
#define ihm07_led_red_off() GPIOB->BSRRH = GPIO_BSRR_BR_2
#define ihm07_led_red_toggle() GPIOB->ODR ^= GPIO_ODR_ODR_2

#define IHM07_ADC_CH_BEMF1 ADC_Channel_13
#define IHM07_ADC_CH_BEMF2 ADC_Channel_8
#define IHM07_ADC_CH_BEMF3 ADC_Channel_7
#define IHM07_ADC_CH_POT ADC_Channel_9
/* set all pins to analog and adc */
void ihm07_adc_init(void);
uint16_t ihm07_adc_read_single_channel(uint8_t channel);






#endif // __IHM07_DRIVER_H
