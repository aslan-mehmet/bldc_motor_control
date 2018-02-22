#ifndef __SERIAL_DRIVER_H
#define __SERIAL_DRIVER_H

void serial_driver_init(void);
int8_t serial_driver_syscheck(void);
void serial_driver_send_byte_poll(uint8_t byt);
void serial_driver_send_buffer_poll(uint8_t *buffer, uint8_t len);

#endif
