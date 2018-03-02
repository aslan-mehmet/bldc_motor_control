#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include "input_fifo.h"
#include "rs232.h"

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
FILE *_file_record = NULL;

static void main_exit(void)
{
        if (_rs232_port_state == RS232_PORT_STATE_OPEN) {
                RS232_CloseComport(_rs232_port_number);
        }

        if (_file_record != NULL) {
                fclose(_file_record);
        }

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

        int rs232_number_of_read_bytes = 0;
        #define RS232_INPUT_BUFFER_LEN 256
        unsigned char rs232_input_buffer[RS232_INPUT_BUFFER_LEN];

        //RS232_SendBuf(_rs232_port_number, buf, len);

	while (1) {
                if (_rs232_port_state == RS232_PORT_STATE_OPEN) {
                        rs232_number_of_read_bytes = RS232_PollComport(_rs232_port_number, rs232_input_buffer, RS232_INPUT_BUFFER_LEN);
                }

                if (rs232_number_of_read_bytes >= 0) {
                        if (_file_record != NULL) {
                                fwrite(rs232_input_buffer, sizeof(unsigned char), rs232_number_of_read_bytes, _file_record);
                        }
                }

                input_fifo_process();
	}

	return -1;
}
