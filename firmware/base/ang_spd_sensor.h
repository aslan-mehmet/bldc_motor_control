#ifndef __ANG_SPD_SENSOR
#define __ANG_SPD_SENSOR

#include "stm32f4xx.h"

void ang_spd_sensor_init(void);
/* measure time between this rot compelete and priv */
void ang_spd_sensor_on_full_rotation_complete(void);
float ang_spd_sensor_get_in_rpm(void);
float ang_spd_sensor_get_in_rad_per_sec(void);
/* sensor give me new value or reading old data all over again*/
uint8_t ang_spd_sensor_exist_new_value(void);

#endif /* __ANG_SPD_SENSOR */
