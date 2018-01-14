/**
 * @file uart.c
 * @author Mehmet ASLAN
 * @date December 3, 2017
 * @copyright Gnu General Public License Version 3 or Later
 */
#include <string.h>
#include "rs232.h"
#include "uart.h"
#include "serial.h"
#include "plotter.h"

#define UART_NAME_BUFFER_SIZE 256 // with null char
/* mode index meanings */
#define UART_SIZE 0
#define UART_PARITY 1
#define UART_STOP_BITS 2
#define UART_INFO_UNIMPORTANT 3 // dont care
/* status states */
#define UART_STATUS_CLOSE 0
#define UART_STATUS_OPEN 1

struct uart_info{
	char name[UART_NAME_BUFFER_SIZE];
	char mode[4];
	int baudrate;
	int port_number;
	int status;
};

static struct uart_info my_uart;

void uart_close(void)
{
	if (my_uart.status == UART_STATUS_OPEN) {
		RS232_CloseComport(my_uart.port_number);
	}

}

void exit_uart(void)
{
	uart_close();
}

void default_uart(void)
{
	strcpy(my_uart.name, "/dev/tty0");
	my_uart.mode[UART_SIZE] = '8'; /* 8 bits comm */
	my_uart.mode[UART_PARITY] = 'N'; /* none */
	my_uart.mode[UART_STOP_BITS] = '1';
	my_uart.mode[UART_INFO_UNIMPORTANT] = 0;
	my_uart.baudrate = 9600;
	my_uart.port_number = get_comport_number(my_uart.name);
	my_uart.status = UART_STATUS_CLOSE;
}

void uart_set_name(char *ptr)
{
	if (strlen(ptr) > UART_NAME_BUFFER_SIZE - 1) {
		puts("fail: uart name too large");
		return;
	}

	strcpy(my_uart.name, ptr);
	my_uart.port_number = get_comport_number(my_uart.name);
}

void uart_set_baudrate(int baud)
{
	my_uart.baudrate = baud;
}

void uart_set_stop_bits(int num)
{
	my_uart.mode[UART_STOP_BITS] = num + '0';
}

static void uart_print_connect_info(void)
{
	puts("uart info");
	puts(my_uart.name);
	printf("baud = %d\n", my_uart.baudrate);
	puts("port opened");
}

extern int8_t sys_check(void);
int uart_connect(void)
{
	if (sys_check()) {
		return -1;
		puts("fail: system variable rep");
	}

	serial_init();

	if (RS232_OpenComport(my_uart.port_number, my_uart.baudrate, my_uart.mode)) {
		puts("fail: rs232_open_comport\n");
		return -1;
	}

	my_uart.status = UART_STATUS_OPEN;

	uart_print_connect_info();
	return 0;
}

/* return # read elements  */
int uart_read(uint8_t *bfr, int bfr_len)
{
	int len;

	if (my_uart.status == UART_STATUS_CLOSE) {
		return -1;
	}

	len = RS232_PollComport(my_uart.port_number, bfr, bfr_len);

	return len;
}

void serial_send_byte(uint8_t byt)
{
	RS232_SendByte(my_uart.port_number, byt);
}

void uart_encode(uint8_t payload_id, uint8_t payload_size, void *payload_ptr)
{
	serial_encode(payload_id,payload_size,payload_ptr);
}

void serial_payload_handler(uint8_t payload_id, uint8_t payload_size
			    , void *payload)
{
	uint16_t u16;
	double d;
	
	switch (payload_id) {
	case 0x32:
		if (payload_size == 2) {
			safe_memory_copy(&u16, payload, payload_size);
			d = (double) u16;

			plotter_put_data(d);
		}
		break;
	}
}
