/*
 * author: Mehmet ASLAN
 * date: January 24, 2018
 * no warranty, no licence agreement
 * use it at your own risk
 */
#include "stm32f4xx.h"
#include "time.h"
#include "ihm07_driver.h"
#include "serial_packet.h"
#include "uart.h"
#include "six_step_hall.h"
#include "ang_spd_sensor.h"
#include "serial_packet_sent_cmd_ids.h"
#include "arm_math.h"

#define SPEED_PID_DEFAULT_KP ((float) 0.1)
#define SPEED_PID_DEFAULT_KI ((float) 0.1)
#define SPEED_PID_DEFAULT_KD ((float) 0)
#define MOTOR_POLE_PAIR_COUNT 5

float _desired_speed = 500;
arm_pid_instance_f32 _speed_pid;

int main(void)
{
        /* 1ms tick */
	if (SysTick_Config(SystemCoreClock / 1000)) {
		while (1)
			;
	}

        /* @arg NVIC_PriorityGroup_2: 2 bits for pre-emption priority */
        /*                            2 bits for subpriority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	/* syscfg reset, so only change interrupt you use*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

        uart_init();
        if (serial_packet_init()) {
                while (1)
                        ;
        }
        ihm07_led_red_init();
        ihm07_l6230_pins_init();
        uint64_t hold_time = get_time();
        /* write after this line */
        uart6_init();

        six_step_hall_init();
        six_step_hall_start();

        uint64_t hold_time_for_pid_controller = get_time();

        _speed_pid.Kp = SPEED_PID_DEFAULT_KP;
        _speed_pid.Ki = SPEED_PID_DEFAULT_KI;
        _speed_pid.Kd = SPEED_PID_DEFAULT_KD;
        /* if any pid param changed, ALWAYS HAVE TO INIT: changes internal params */
        arm_pid_init_f32(&_speed_pid, 1);

        while (1) {
                if (get_time() - hold_time_for_pid_controller > 100) {
                        hold_time_for_pid_controller = get_time();

                        static float current_speed, error_speed, pwm_val;
                        /* returns single hall step in rpm */
                        current_speed = ang_spd_sensor_get_in_rpm();
                        /* mechanical shaft speed */
                        current_speed /= ((float)MOTOR_POLE_PAIR_COUNT * 6.0);
                        error_speed = _desired_speed - current_speed;
                        pwm_val = arm_pid_f32(&_speed_pid, error_speed);

                        if (pwm_val < 0) {
                                pwm_val = 0;
                        } else if (pwm_val > SIX_STEP_MAX_PWM_VAL) {
                                pwm_val = SIX_STEP_MAX_PWM_VAL;
                        }

                        six_step_hall_set_pwm_val((uint16_t) pwm_val);

                        static uint8_t uart6_comm_header[4] = {0x00, 0x01, 0x02, 0x03};
                        uart6_send_buffer_poll(uart6_comm_header, 4);
                        uart6_send_buffer_poll((uint8_t *)(&_desired_speed), 4);
                        uart6_send_buffer_poll((uint8_t *)(&current_speed), 4);
                        uart6_send_buffer_poll((uint8_t *)(&error_speed), 4);
                        uart6_send_buffer_poll((uint8_t *)(&pwm_val), 4);

                        uint16_t u16 = (uint16_t) pwm_val;
                        serial_packet_encode_poll(PRINT_PWM_VAL, sizeof(u16), &u16);
                }

                /* dont touch this lines */
                if (get_time() - hold_time > 100) {
                        hold_time = get_time();
                        ihm07_led_red_toggle();
                }
                serial_packet_flush();
        }
}

/* dont touch this lines */
void serial_packet_print(uint8_t byt)
{
        uart_send_byte_poll(byt);
}
