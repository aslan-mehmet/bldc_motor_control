/**
 * @file main.c
 * @author Mehmet ASLAN
 * @date December 3, 2017
 * @copyright Gnu General Public License Version 3 or Later
 */
#include "folder.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <time.h>
#include "log.h"
#include "uart.h"
#include "server.h"
#include "serial.h"
#include "plotter.h"

/* call all exit function in every src file */
void main_exit(void)
{
	exit_plotter();
	exit_server();
	exit_uart();
	exit_log();
	exit_folder();
	exit(0);
}

static void signal_handler(int signum)
{
	if (signum == SIGINT) {
		main_exit();
	}
}

static void delay_ms(int ms)
{
	struct timespec t;

	t.tv_sec = 0;
	/* max 1 sec */
	t.tv_nsec = 1e+6;

	for (int i = 0; i < ms; ++i) {
		nanosleep(&t, NULL);
	}
}

extern char help_str[];
int main(int argc, char **argv)
{
	fclose(stdin);
	fclose(stderr);
	uart_set_default();

	/** used for get options  */
	char str_buf[256];
	int func_return;
	int number;

	/* change directory to temp folder, after this all relative path referenced this*/
	if (chdir_temp_folder() != 0) {
		puts("fail: chdir_temp_folder");
	        return -1;
	}

	signal(SIGINT, signal_handler);

	struct option long_options[] = {
		{"baudrate", required_argument, NULL, 'b'},
		{"port_name", required_argument, NULL, 'p'},
		{"stop_bits", required_argument, NULL, 's'},
		{"keep_temp_files", no_argument, NULL, 'k'},
		{"log_received_data", no_argument, NULL, 'l'},
		{"help", no_argument, NULL, 'h'},
		/* You have to terminate the longopts array with an entry that is all zeros,
		   otherwise getopt_long doesn't know when it ends.
		   Your code is crashing because getopt_long is just iterating through
		   random memory at that point because it has fallen off the end of longopts.
		   https://stackoverflow.com/questions/30956582/segfault-for-invalid-long-option */
		{NULL, 0, NULL, 0}
	};

	while ((func_return = getopt_long(argc, argv, "b:p:s:klh", long_options, NULL)) != EOF) {
		switch (func_return) {
		case 'b':
			sscanf(optarg, "%d", &number);
			uart_set_baudrate(number);
			break;
		case 'p':
			sscanf(optarg, "%s", str_buf);
			uart_set_name(str_buf);
			break;
		case 's':
			sscanf(optarg, "%d", &number);
			uart_set_stop_bits(number);
			break;
		case 'k':
			keep_temp_folder();
			break;
		case 'h':
			puts(help_str);
			main_exit();
			break;
		case 'l':
			start_log();
			break;
		}
	}

	#define BIN_BFR_SIZE 256
	uint8_t bin_bfr[BIN_BFR_SIZE];
	uart_connect();

	if (server_create()) {
		return -1;
	}

	plotter_create(1000, 0, 4095);

	while (1) {
		func_return = uart_loop(bin_bfr, BIN_BFR_SIZE);

		loop_log(bin_bfr, func_return);
		server_loop();
		delay_ms(10);

		plotter_loop();

#ifdef __PLOTTER_SERIAL_PACKET_DEBUG
/* connect ttyUSB chip RX TX PINS */		
static uint16_t data = 0;
data+=100;
if (data > 4096)
	data = 0;
		
uart_encode(0x32, sizeof(data), &data);
#endif
	}

	/* should not come here */
	return -1;
}
