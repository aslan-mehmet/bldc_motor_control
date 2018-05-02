#ifndef __SIX_STEP_HALL_H
#define __SIX_STEP_HALL_H

#include "ihm07_driver.h"
#include "stm32f4xx.h"

void six_step_hall_init(void);
/* make motor spin */
void six_step_hall_start(void);
void six_step_hall_stop(void);

#define SIX_STEP_MAX_PWM_VAL PWM_MAX_VAL
/* default pwm_val 0 */
void six_step_hall_set_pwm_val(uint16_t pwm_val);

#define SIX_STEP_HALL_DIRECTION_CCW 1
#define SIX_STEP_HALL_DIRECTION_CW 0
/* default ccw */
void six_step_hall_set_direction(uint8_t SIX_STEP_HALL_DIRECTION_x);

/* READONLY */
extern uint8_t _six_step_hall_current_step;

#endif /* __SIX_STEP_HALL_H */
