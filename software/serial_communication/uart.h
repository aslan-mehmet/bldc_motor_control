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

void uart_close(void);
void exit_uart(void);
void uart_set_default(void);
void uart_set_name(char *ptr);
void uart_set_baudrate(int baud);
void uart_set_stop_bits(int num);
int uart_connect(void);
int uart_read(uint8_t *bfr, int bfr_len);
int uart_loop(uint8_t *bfr, int bfr_len);
void uart_encode(uint8_t payload_id, uint8_t payload_size, void *payload_ptr);
#endif
