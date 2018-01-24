/**
 * @file uart.h
 * @author Mehmet ASLAN
 * @date December 3, 2017
 * @copyright Gnu General Public License Version 3 or Later
 * @brief handles pc serial port
 */
#ifndef __UART_H
#define __UART_H

#include <stdint.h>
/* close uart restore old settings */
void uart_close(void);
int uart_connect(void);
/* closing uart at the end of program */
void exit_uart(void);
/* default vals for uart */
void default_uart(void);

int uart_connect(void);

/* give path ex. /dev/ttyUSB0 */
void uart_set_name(char *ptr);
/* avaliable bauds in help string */
void uart_set_baudrate(int baud);
void uart_set_stop_bits(int num);


int uart_read(uint8_t *bfr, int bfr_len);
void uart_encode(uint8_t payload_id, uint8_t payload_size, void *payload_ptr);
#endif
