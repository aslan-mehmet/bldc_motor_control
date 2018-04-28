#include "serial_packet.h"
#include "six_step_hall.h"
#include "arm_math.h"

union VARS {
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
        int8_t i8;
        int16_t i16;
        int32_t i32;
        int64_t i64;
        float f;
        double d;
};

#define MOTOR_SET_STATE 0
#define MOTOR_SET_PWM 1
#define MOTOR_SET_DIRECTION 2
#define SPEED_PID_SET_KP 3
#define SPEED_PID_SET_KI 4
#define SPEED_PID_SET_KD 5
#define MOTOR_SET_SPEED_IN_RPM 6

#ifdef SPEED_PID_PRESENT
extern float _desired_speed;
extern arm_pid_instance_f32 _speed_pid;
#endif /* SPEED_PID_PRESENT */

void serial_packet_handler(uint8_t payload_id, uint8_t payload_size
			    , void *payload)
{
        union VARS var;

        if (sizeof(var) >= payload_size) {
                safe_memory_copy(&var.u8, payload, payload_size);
        }

        switch (payload_id) {
        case MOTOR_SET_STATE:
		if (var.u8) {
                	six_step_hall_start();
		} else {
                	six_step_hall_stop();
		}
                break;
        case MOTOR_SET_PWM:
                six_step_hall_set_pwm_val(var.u16);
                break;
        case MOTOR_SET_DIRECTION:
                six_step_hall_set_direction(var.u8);
                break;
        #ifdef SPEED_PID_PRESENT
        case SPEED_PID_SET_KP:
                _speed_pid.Kp = var.f;
                arm_pid_init_f32(&_speed_pid, 1);
                break;
        case SPEED_PID_SET_KI:
                _speed_pid.Ki = var.f;
                arm_pid_init_f32(&_speed_pid, 1);
                break;
        case SPEED_PID_SET_KD:
                _speed_pid.Kd = var.f;
                arm_pid_init_f32(&_speed_pid, 1);
                break;
        case MOTOR_SET_SPEED_IN_RPM:
                _desired_speed = var.f;
                break;
        #endif /* SPEED_PID_PRESENT */
        }
}
