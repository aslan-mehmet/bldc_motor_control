#include "serial_packet.h"
#include <stdio.h>
#include <time.h>

int main(int argc, char **argv)
{
        if (serial_packet_init()) {
		return 1;
	}

	FILE *f = fopen("received.bin", "rb");
	uint8_t u8;


        while (fread(&u8, sizeof(u8), 1, f) > 0) {
		serial_packet_read(u8);
		serial_packet_flush();
	}

	fclose(f);        
        return 0;
}

void serial_packet_print(uint8_t byt)
{
	printf("%x ", byt);
}
