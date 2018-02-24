#ifndef __SIX_STEP_HALL_H
#define __SIX_STEP_HALL_H

#include "ihm07_driver.h"
#include "stm32f4xx.h"

void six_step_hall_init(void);
void six_step_hall_start(void);
#define six_step_hall_stop() ihm07_l6230_disable()

void six_step_hall_set_speed(uint16_t val);

#define SIX_STEP_HALL_DIRECTION_CCW 1
#define SIX_STEP_HALL_DIRECTION_CW 0
void six_step_hall_set_direction(uint8_t SIX_STEP_HALL_DIRECTION_x);

#endif /* __SIX_STEP_HALL_H */
