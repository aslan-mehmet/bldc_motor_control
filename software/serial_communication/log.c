/**
 * @file log.c
 * @author Mehmet ASLAN
 * @date December 3, 2017
 * @copyright Gnu General Public License Version 3 or Later
 * @brief write data received from uc to a file
 */
#include <stdio.h>
#include <stdint.h>

/* file to write on */
static FILE *_serial_rx_file = NULL;
/* stop writing */
void close_log(void)
{
	if (_serial_rx_file != NULL) {
		fclose(_serial_rx_file);
	}

}

void exit_log(void)
{
	close_log();
}
/* can start any point */
void start_log(void)
{
	/* append access so it can be closed and reopened */
	_serial_rx_file = fopen("serial_rx_log_bin", "ab");

	if (_serial_rx_file == NULL) {
		puts("fail: open serial_rx_log_bin\n");
	}
}
/* if logging not started nothing will happen */
void write_log(uint8_t *ptr, int len)
{
	if (_serial_rx_file != NULL) {
		fwrite(ptr, sizeof(uint8_t), len, _serial_rx_file);
	}
}

void log_loop(uint8_t *ptr, int len)
{
	if (len <= 0) {
		return;
	}

	write_log(ptr, len);
}
