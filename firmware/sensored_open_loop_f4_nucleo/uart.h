#ifndef __UART_H
#define __UART_H

void uart_init(void);
void uart_send_byte_poll(uint8_t byt);
void uart_send_buffer_poll(uint8_t *buffer, uint8_t len);

/* config: auto start when all buffer send, only tx mode */
void uart6_stream_init(uint8_t *buf, uint8_t buf_size);
/**
 * dont immediately start. use one of this first
 * uart6_stream_init
 * uart6_stream_single_config_dma
 */
void uart6_stream_start(void);
void uart6_stream_stop(void);

void uart6_stream_single_periph_init(void);
/* implicity stops uart6 if not closed */
void uart6_stream_single_config_dma(uint8_t *buf, uint8_t buf_size);

#endif
