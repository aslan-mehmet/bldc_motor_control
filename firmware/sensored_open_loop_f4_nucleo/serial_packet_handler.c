#include "serial_packet.h"
#include "six_step_hall.h"

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
        }
}
