#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f4xx.h"

/* first use motor init to set pins properly */
void motor_init(void);
/* enable motor driver circuit */
void motor_start(void);
void motor_stop(void);
/* max 1e3 */
void motor_set_pwm(uint16_t val);

#endif
