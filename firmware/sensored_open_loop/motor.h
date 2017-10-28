#ifndef __MOTOR_H
#define __MOTOR_H
#include <stdint.h>

void motor_init(void);
void motor_commutate(uint8_t step_number);
void motor_set_speed(uint16_t rpm);

#endif	/* __MOTOR_H */
