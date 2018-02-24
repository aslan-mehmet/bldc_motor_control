#include "six_step_hall.h"

void ihm07_hall_state_change_callback(void);

static uint16_t _speed = 0;
static uint8_t _direction = SIX_STEP_HALL_DIRECTION_CCW;

void six_step_hall_init(void)
{
        ihm07_l6230_pins_init();
        ihm07_hall_pins_init();
        ihm07_hall_interrupt_init();
        ihm07_hall_interrupt_connection_state(ENABLE);
        ihm07_pwm_and_pins_init();
}
void six_step_hall_start(void)
{
        ihm07_l6230_enable();
        ihm07_hall_state_change_callback();
}
static void six_step_commutate_cw(uint8_t state)
{
        ihm07_pwm_set_ch1(0);
        ihm07_pwm_set_ch2(0);
        ihm07_pwm_set_ch3(0);
        ihm07_l6230_ch_all_disable();

        switch (state) {
        case 6:
                ihm07_l6230_ch2_enable();
                ihm07_l6230_ch3_enable();
                ihm07_pwm_set_ch2(_speed);
                break;
        case 4:
                ihm07_l6230_ch1_enable();
                ihm07_l6230_ch3_enable();
                ihm07_pwm_set_ch1(_speed);
                break;
        case 5:
                ihm07_l6230_ch1_enable();
                ihm07_l6230_ch2_enable();
                ihm07_pwm_set_ch1(_speed);
                break;
        case 1:
                ihm07_l6230_ch2_enable();
                ihm07_l6230_ch3_enable();
                ihm07_pwm_set_ch3(_speed);
                break;
        case 3:
                ihm07_l6230_ch1_enable();
                ihm07_l6230_ch3_enable();
                ihm07_pwm_set_ch3(_speed);
                break;
        case 2:
                ihm07_l6230_ch1_enable();
                ihm07_l6230_ch2_enable();
                ihm07_pwm_set_ch2(_speed);
                break;
        }
}

static void six_step_commutate_ccw(uint8_t state)
{
        ihm07_pwm_set_ch1(0);
        ihm07_pwm_set_ch2(0);
        ihm07_pwm_set_ch3(0);
        ihm07_l6230_ch_all_disable();

        switch (state) {
        case 5:
                ihm07_l6230_ch1_enable();
                ihm07_l6230_ch2_enable();
                ihm07_pwm_set_ch2(_speed);
                break;
        case 4:
                ihm07_l6230_ch1_enable();
                ihm07_l6230_ch3_enable();
                ihm07_pwm_set_ch3(_speed);
                break;
        case 6:
                ihm07_l6230_ch2_enable();
                ihm07_l6230_ch3_enable();
                ihm07_pwm_set_ch3(_speed);
                break;
        case 2:
                ihm07_l6230_ch1_enable();
                ihm07_l6230_ch2_enable();
                ihm07_pwm_set_ch1(_speed);
                break;
        case 3:
                ihm07_l6230_ch1_enable();
                ihm07_l6230_ch3_enable();
                ihm07_pwm_set_ch1(_speed);
                break;
        case 1:
                ihm07_l6230_ch2_enable();
                ihm07_l6230_ch3_enable();
                ihm07_pwm_set_ch2(_speed);
                break;
        }
}

void ihm07_hall_state_change_callback(void)
{
        uint8_t state = ihm07_hall_read();

        if (_direction == SIX_STEP_HALL_DIRECTION_CW) {
                six_step_commutate_cw(state);
        } else {
                six_step_commutate_ccw(state);
        }
}

void six_step_hall_set_speed(uint16_t val)
{
        _speed = val;
}

void six_step_hall_set_direction(uint8_t SIX_STEP_HALL_DIRECTION_x)
{
        _direction = SIX_STEP_HALL_DIRECTION_x;
}
