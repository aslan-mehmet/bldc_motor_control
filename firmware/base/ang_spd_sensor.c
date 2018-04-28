#include "ang_spd_sensor.h"

#define SENSOR_TIM TIM2
#define CONSTANT_PI (3.1415926535F)
/* unit time = (42*10^6)^-1 sec = 23.81 ns */
#define PERIOD_UNIT_TIME (23.80952381e-9F)
/* timer overflows in 10 ms */
/* min rpm val can be measured 6000 */
#define SENSOR_TIM_PERIOD 420000
static uint32_t _full_rotation_period = SENSOR_TIM_PERIOD;
static uint8_t _new_full_rotation_period_flag = 0;
static uint8_t _ang_spd_sensor_init_complete = 0;

void ang_spd_sensor_init(void)
{
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
        TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
        NVIC_InitTypeDef NVIC_InitStructure;

        TIM_TimeBaseStructure.TIM_Prescaler = 0;
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
        TIM_TimeBaseStructure.TIM_Period = SENSOR_TIM_PERIOD;
        TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
        TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
        TIM_TimeBaseInit(SENSOR_TIM, &TIM_TimeBaseStructure);

        NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

        TIM_ITConfig(SENSOR_TIM, TIM_IT_Update, ENABLE);

        TIM_SetCounter(SENSOR_TIM, 0);
        TIM_Cmd(SENSOR_TIM, ENABLE);
        _ang_spd_sensor_init_complete = 1;
}

void ang_spd_sensor_timer_overflow_callback(void)
{
        _full_rotation_period = SENSOR_TIM_PERIOD;
        _new_full_rotation_period_flag = 1;
}

void ang_spd_sensor_on_full_rotation_complete(void)
{
        if (_ang_spd_sensor_init_complete == 0) {
                return;
        }

        _full_rotation_period = TIM_GetCounter(SENSOR_TIM);

        TIM_SetCounter(SENSOR_TIM, 0);

        _new_full_rotation_period_flag = 1;
        /* uev received before rotation complete */
        if (_full_rotation_period == 0) {
                _full_rotation_period = SENSOR_TIM_PERIOD;
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
