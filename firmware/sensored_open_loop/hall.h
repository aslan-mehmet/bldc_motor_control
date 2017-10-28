#ifndef __HALL_H
#define __HALL_H
#include <stdint.h>

void hall_init(void);
/* return speed in rpm */
uint16_t hall_get_speed(void);
#endif
