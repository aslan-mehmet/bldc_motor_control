#include "main.h"
#include "folder.h"
#include "rs232.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

FILE *_stderr = NULL;
static int _keep_temp_files = 0;
static int _is_serial_port_open = 0;
static int _serial_port_number = -1;

/* write all received serial data */
static FILE *_serial_rx_file = NULL;
static int _serial_rx_file_logging = 0;

void main_exit(void)
{
	/* close all files, set port to initial vals */
	if (_stderr != NULL) {
		fclose(_stderr);
	}

	if (_serial_rx_file != NULL) {
		fclose(_serial_rx_file);
	}
        
	if (_keep_temp_files == 0 && remove_temp_folder() != 0) {
		puts("fail: remove_temp_folder");
	}

	if (_is_serial_port_open) {
		RS232_CloseComport(_serial_port_number);
	}

	exit(0);
}

/* signal handler, die gracefully */
static void signal_handler(int signum)
{
	if (signum == SIGINT) {
		main_exit();
	}
}

extern char help_str[];
int main(int argc, char **argv)
{
	int func_return;
	char serial_port_name[256] = "";
	char serial_mode[] = {'8', 'N', '1', 0};
	#define SERIAL_BUF_SIZE 100
	unsigned char serial_buf[SERIAL_BUF_SIZE];
	int serial_baudrate = -1;
	
	/* change directory to temp folder */
	if (chdir_temp_folder() != 0) {
		puts("fail: chdir_temp_folder");
	        return -1;
	}
        
	fclose(stderr);
	/* use this to log error */
	_stderr = fopen("error_log", "w");
	
	if (_stderr == NULL) {
		puts("fail: open error_log");
		return -1;
	}
	/* after this point, only use _stderr to log */

	signal(SIGINT, signal_handler);
	
	/* handle options */
	struct option long_options[] = {
		{"baudrate", required_argument, NULL, 'b'},
		{"port_name", required_argument, NULL, 'p'},
		{"stop_bits", required_argument, NULL, 's'},
		{"keep_temp_files", no_argument, NULL, 'k'},
		{"log_received_data", no_argument, NULL, 'l'},
		{"help", no_argument, NULL, 'h'}
	};

	while ((func_return = getopt_long(argc, argv, "b:p:s:", long_options, NULL)) != EOF) {
		int tmp;
		switch (func_return) {
		case 'b':
			sscanf(optarg, "%d", &serial_baudrate);
			break;
		case 'p':
			sscanf(optarg, "%s", serial_port_name);
			_serial_port_number = get_comport_number(serial_port_name);
			break;
		case 's':
			sscanf(optarg, "%d", &tmp);
			serial_mode[2] = (char) tmp + '0';
			break;
		case 'k':
			_keep_temp_files = 1;
			break;
		case 'h':
			puts(help_str);
			main_exit();
			break;
		case 'l':
			_serial_rx_file_logging = 1;
			break;
		}
	}

	if (_serial_rx_file_logging) {
		_serial_rx_file = fopen("serial_rx_log_bin", "wb");

		if (_serial_rx_file == NULL) {
			fprintf(_stderr, "fail: open serial_rx_log_bin\n");
			_serial_rx_file_logging = 0;
		}
	}
	
	if (RS232_OpenComport(_serial_port_number, serial_baudrate, serial_mode)) {
		fprintf(_stderr, "fail: rs232_open_comport\n");
		main_exit();
	}

	_is_serial_port_open = 1;
	puts("serial port opened");
	
	/* connect port */
	while (1) {
		func_return = RS232_PollComport(_serial_port_number, serial_buf, SERIAL_BUF_SIZE);

		if (_serial_rx_file_logging) {
			fwrite(serial_buf, sizeof(unsigned char), func_return, _serial_rx_file);
		}
	}

	/* should not come here */
	return -1;
}
