#ifndef __UART_H
#define __UART_H

void uart_init(void);
void uart_send_byte_poll(uint8_t byt);
void uart_send_buffer_poll(uint8_t *buffer, uint8_t len);

#endif
