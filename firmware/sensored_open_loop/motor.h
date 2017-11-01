#ifndef __MOTOR_H
#define __MOTOR_H
#include "stm32f4xx.h"

void motor_start(void);
void motor_stop(void);
void motor_init(void);
#define PWM_MAX ((uint16_t) 2000)
void motor_set_pwm(uint16_t val);
void motor_commutate(uint8_t step_number);

#endif
