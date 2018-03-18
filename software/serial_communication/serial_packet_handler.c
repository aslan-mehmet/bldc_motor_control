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
static FILE *f = NULL;

void serial_packet_handler_record_open(void)
{
        f = fopen("serial_packet_handler_record.csv", "w");
}

void serial_packet_handler_record_close(void)
{
        if (f != NULL) {
                fclose(f);
                f = NULL;
        }
}

void serial_packet_handler_record_comment(char *comment_str)
{
        if (f != NULL) {
                fprintf(f, "#%s\n", comment_str);
        }
}

void print_bemfs(uint8_t payload_id, uint8_t payload_size, void *payload)
{
        uint16_t bemfs[3];
        uint64_t time = 0;

        safe_memory_copy(&time, payload, sizeof(time));
        safe_memory_copy(bemfs, payload + sizeof(time), sizeof(bemfs));

        if (f != NULL) {
                fprintf(f, "%d, %d, %d, %d, %d\n", (int)payload_id, (int)time, (int)bemfs[0], (int)bemfs[1], (int)bemfs[2]);
        }
}

#define PRINT_BEMFS 0
#define PRINT_SPD_RPM 1
void serial_packet_handler(uint8_t payload_id, uint8_t payload_size
			    , void *payload)
{
        union VARS var;

        if (sizeof(var) >= payload_size) {
                safe_memory_copy(&var.u8, payload, payload_size);
        }

        switch (payload_id) {
        case PRINT_BEMFS:
                print_bemfs(payload_id, payload_size, payload);
        break;
	case PRINT_SPD_RPM:
		printf("rpm: %f\n", var.f);
	break;
        }
}
