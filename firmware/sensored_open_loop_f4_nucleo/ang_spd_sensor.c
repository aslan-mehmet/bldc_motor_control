#include "ang_spd_sensor.h"

#define SENSOR_TIM TIM2
#define CONSTANT_PI (3.1415926535F)
/* slowest val. unit time = (42*10^6)^-1 sec = 23.81 ns */
#define PERIOD_UNIT_TIME (23.80952381e-9F)
static uint32_t _full_rotation_period = UINT32_MAX;
static uint8_t _new_full_rotation_period_flag = 0;
static uint8_t _ang_spd_sensor_init_complete = 0;

void ang_spd_sensor_init(void)
{
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
        TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

        TIM_TimeBaseStructure.TIM_Prescaler = 0;
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
        TIM_TimeBaseStructure.TIM_Period = UINT32_MAX;
        TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
        TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
        TIM_TimeBaseInit(SENSOR_TIM, &TIM_TimeBaseStructure);

        /* TODO: set interrupt say timeout waiting rotation */

        TIM_SelectOnePulseMode(SENSOR_TIM, TIM_OPMode_Single);
        TIM_SetCounter(SENSOR_TIM, 0);
        _ang_spd_sensor_init_complete = 1;
}

void ang_spd_sensor_on_full_rotation_complete(void)
{
        if (_ang_spd_sensor_init_complete == 0) {
                return;
        }

        _full_rotation_period = TIM_GetCounter(SENSOR_TIM);

        TIM_Cmd(SENSOR_TIM, DISABLE);
        TIM_SetCounter(SENSOR_TIM, 0);
        TIM_Cmd(SENSOR_TIM, ENABLE);

        _new_full_rotation_period_flag = 1;
        /* uev received before rotation complete */
        if (_full_rotation_period == 0) {
                _full_rotation_period = UINT32_MAX;
        }
}

float ang_spd_sensor_get_in_rpm(void)
{
        float period_in_min = _full_rotation_period * PERIOD_UNIT_TIME / 60.0;
        float rpm = 1.0 / period_in_min;

        _new_full_rotation_period_flag = 0;
        return rpm;
}

float ang_spd_sensor_get_in_rad_per_sec(void)
{
        float period_in_sec = _full_rotation_period * PERIOD_UNIT_TIME;
        float rad_per_sec = 2 * CONSTANT_PI / period_in_sec;

        _new_full_rotation_period_flag = 0;
        return rad_per_sec;
}

uint8_t ang_spd_sensor_exist_new_value(void)
{
        return _new_full_rotation_period_flag;
}
