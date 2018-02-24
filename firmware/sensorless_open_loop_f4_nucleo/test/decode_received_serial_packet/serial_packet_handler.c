#include "serial_packet.h"
#include <stdio.h>
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

void serial_packet_handler(uint8_t payload_id, uint8_t payload_size
			    , void *payload)
{
        union VARS var;

        if (sizeof(var) >= payload_size) {
                safe_memory_copy(&var.u8, payload, payload_size);
        }

	switch (payload_id) {
	case 1:
		printf("%f\n", (float)var.u8);
		break;
	case 2:
		printf("%f\n", (float)var.u16);
		break;
	case 3:
		printf("%f\n", (float)var.u32);
		break;
	case 4:
		printf("%f\n", (float)var.u64);
		break;
	case 5:
		printf("%f\n", (float)var.i8);
		break;
	case 6:
		printf("%f\n", (float)var.i16);
		break;
	case 7:
		printf("%f\n", (float)var.i32);
		break;
	case 8:
		printf("%f\n", (float)var.i64);
		break;
	case 9:
		printf("%f\n", (float)var.f);
		break;
	case 10:
		printf("%f\n", (float)var.d);
		break;
	}
}
