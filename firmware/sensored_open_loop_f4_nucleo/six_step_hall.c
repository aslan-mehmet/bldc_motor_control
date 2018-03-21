#include "six_step_hall.h"
#include "ang_spd_sensor.h"

void ihm07_hall_state_change_callback(void);

static uint16_t _pwm_val = 0;
static uint8_t _direction = SIX_STEP_HALL_DIRECTION_CCW;
#define SIX_STEP_STATE_ACTIVE 1
#define SIX_STEP_STATE_INACTIVE 0
static uint8_t _six_step_state = SIX_STEP_STATE_INACTIVE;

void six_step_hall_init(void)
{
        ihm07_l6230_pins_init();
        ihm07_hall_pins_init();
        ihm07_hall_interrupt_init();
        ihm07_hall_interrupt_connection_state(ENABLE);
        ihm07_pwm_and_pins_init();
        ihm07_pwm_state(ENABLE);
}
void six_step_hall_start(void)
{
        ihm07_l6230_enable();
        ihm07_hall_state_change_callback();
        _six_step_state = SIX_STEP_STATE_ACTIVE;
}

void six_step_hall_stop(void)
{
        _six_step_state = SIX_STEP_STATE_INACTIVE;
        ihm07_l6230_disable();
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
                ihm07_pwm_set_ch2(_pwm_val);
                break;
        case 4:
                ihm07_l6230_ch1_enable();
                ihm07_l6230_ch3_enable();
                ihm07_pwm_set_ch1(_pwm_val);
                break;
        case 5:
                ihm07_l6230_ch1_enable();
                ihm07_l6230_ch2_enable();
                ihm07_pwm_set_ch1(_pwm_val);
                break;
        case 1:
                ihm07_l6230_ch2_enable();
                ihm07_l6230_ch3_enable();
                ihm07_pwm_set_ch3(_pwm_val);
                break;
        case 3:
                ihm07_l6230_ch1_enable();
                ihm07_l6230_ch3_enable();
                ihm07_pwm_set_ch3(_pwm_val);
                break;
        case 2:
                ihm07_l6230_ch1_enable();
                ihm07_l6230_ch2_enable();
                ihm07_pwm_set_ch2(_pwm_val);
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
                ihm07_pwm_set_ch2(_pwm_val);
                break;
        case 4:
                ihm07_l6230_ch1_enable();
                ihm07_l6230_ch3_enable();
                ihm07_pwm_set_ch3(_pwm_val);
                break;
        case 6:
                ihm07_l6230_ch2_enable();
                ihm07_l6230_ch3_enable();
                ihm07_pwm_set_ch3(_pwm_val);
                break;
        case 2:
                ihm07_l6230_ch1_enable();
                ihm07_l6230_ch2_enable();
                ihm07_pwm_set_ch1(_pwm_val);
                break;
        case 3:
                ihm07_l6230_ch1_enable();
                ihm07_l6230_ch3_enable();
                ihm07_pwm_set_ch1(_pwm_val);
                break;
        case 1:
                ihm07_l6230_ch2_enable();
                ihm07_l6230_ch3_enable();
                ihm07_pwm_set_ch2(_pwm_val);
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

        if (state == 6) {
                ang_spd_sensor_on_full_rotation_complete();
        }
}

void six_step_hall_set_pwm_val(uint16_t val)
{
        _pwm_val = val;
        if (_six_step_state == SIX_STEP_STATE_ACTIVE) {
                ihm07_hall_state_change_callback();
        }
}

void six_step_hall_set_direction(uint8_t SIX_STEP_HALL_DIRECTION_x)
{
        _direction = SIX_STEP_HALL_DIRECTION_x;
        if (_six_step_state == SIX_STEP_STATE_ACTIVE) {
                ihm07_hall_state_change_callback();
        }
}
