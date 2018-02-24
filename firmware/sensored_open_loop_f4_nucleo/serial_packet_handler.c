#include "serial_packet.h"
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
}
