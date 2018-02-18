#ifndef __UART_H
#define __UART_H

#include "stm32f4xx.h"

void uart_init(void);
void uart_send_poll(uint8_t d);
void uart_send_poll_bfr(uint8_t *p, uint8_t len);
#endif
