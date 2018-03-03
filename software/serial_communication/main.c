#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include "input_fifo.h"
#include "rs232.h"
#include "serial_packet.h"
#include "serial_packet_handler.h"

int _rs232_port_number = -1;
#define RS232_PORT_MODE_DATABITS_INDEX 0
#define RS232_PORT_MODE_PARITY_INDEX 1
#define RS232_PORT_MODE_STOP_BITS_INDEX 2
#define RS232_PORT_MODE_DONT_USE_INDEX 3
char _rs232_port_mode[4] = {'8', 'N', '1', 0};
int _rs232_port_baudrate = -1;
#define RS232_PORT_STATE_OPEN 1
#define RS232_PORT_STATE_CLOSE 0
int _rs232_port_state = RS232_PORT_STATE_CLOSE;
#define RS232_PORT_LISTENING_ACTIVE 1
#define RS232_PORT_LISTENING_INACTIVE 0
int _rs232_port_listening_status = RS232_PORT_LISTENING_ACTIVE;
FILE *_file_record = NULL;

void rs232_port_listening_status_set(int RS232_PORT_LISTENING_x)
{
        _rs232_port_listening_status = RS232_PORT_LISTENING_x;
}

void main_exit(void)
{
        if (_rs232_port_state == RS232_PORT_STATE_OPEN) {
                RS232_CloseComport(_rs232_port_number);
        }

        if (_file_record != NULL) {
                fclose(_file_record);
                _file_record = NULL;
        }

        serial_packet_handler_record_close();

        input_fifo_close();
        putchar('\n');
        exit(0);
}

static void signal_handler(int signum)
{
	if (signum == SIGINT) {
                main_exit();
	}
}

extern const char help_str[];
int main(int argc, char **argv)
{
	fclose(stdin);
	fclose(stderr);

	signal(SIGINT, signal_handler);

	struct option long_options[] = {
		{"baudrate", required_argument, NULL, 'a'},
		{"port_name", required_argument, NULL, 'b'},
		{"stop_bits", required_argument, NULL, 'c'},
                {"record", required_argument, NULL, 'd'},
		{"help", no_argument, NULL, 'h'},
		/* You have to terminate the longopts array with an entry that is all zeros
		   https://stackoverflow.com/questions/30956582/segfault-for-invalid-long-option */
		{NULL, 0, NULL, 0}
	};

        int getopt_char;
	while ((getopt_char = getopt_long(argc, argv, "a:b:c:d:h", long_options, NULL)) != EOF) {
                int tmp;
                switch (getopt_char) {
                case 'a':
                        sscanf(optarg, "%d", &_rs232_port_baudrate);
                        break;
                case 'b':
                        _rs232_port_number = RS232_GetPortnr(optarg);
                        break;
                case 'c':
                        sscanf(optarg, "%c", &_rs232_port_mode[RS232_PORT_MODE_STOP_BITS_INDEX]);
                        break;
                case 'd':
                        _file_record = fopen(optarg, "wb");
                        if (_file_record == NULL) {
                                puts("fail: file record");
                                main_exit();
                        }
                        break;
                case 'h':
                        puts(help_str);
                        main_exit();
                        break;
		}
	}

        if (input_fifo_open()) {
                main_exit();
        }

        _rs232_port_state = RS232_PORT_STATE_OPEN;
        if (RS232_OpenComport(_rs232_port_number, _rs232_port_baudrate, _rs232_port_mode)) {
                puts("fail: rs232 port open");
                _rs232_port_state = RS232_PORT_STATE_CLOSE;
        }

        if (serial_packet_init()) {
                puts("fail: serial packet init");
                main_exit();
        }

        serial_packet_handler_record_open();

        int rs232_number_of_read_bytes = 0;
        #define RS232_INPUT_BUFFER_LEN SERIAL_PACKET_MAX_PAYLOAD_SIZE*SERIAL_PACKET_MAX_PACKET_COUNT
        unsigned char rs232_input_buffer[RS232_INPUT_BUFFER_LEN];

	while (1) {
                input_fifo_process();

                if (_rs232_port_state == RS232_PORT_STATE_OPEN) {
                        rs232_number_of_read_bytes = RS232_PollComport(_rs232_port_number, rs232_input_buffer, RS232_INPUT_BUFFER_LEN);
                }

                if (rs232_number_of_read_bytes >= 0 && _rs232_port_listening_status == RS232_PORT_LISTENING_ACTIVE) {
                        if (_file_record != NULL) {
                                fwrite(rs232_input_buffer, sizeof(unsigned char), rs232_number_of_read_bytes, _file_record);
                        }

                        for (int i = 0; i < rs232_number_of_read_bytes; ++i) {
                                serial_packet_read(rs232_input_buffer[i]);
                        }
                }

                serial_packet_flush();
	}

	return -1;
}

void serial_packet_print(uint8_t byt)
{
        if (_rs232_port_state == RS232_PORT_STATE_OPEN) {
                RS232_SendBuf(_rs232_port_number, &byt, 1);
        }
}
