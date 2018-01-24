/**
 * @file serial.h
 * @author Mehmet ASLAN
 * @date November 19, 2017
 * @copyright Gnu General Public License Version 3 or Later
 * @brief
 * this is a serial communication protocol\n
 * there 2 different characters : escaped characters and normal characters\n
 * data packaged to send via serial byte communication\n
 * single package character order(size in byte):\n
 * START(2) PAYLOAD_ID(1) PAYLOAD_SIZE(1) PAYLOAD(PAYLOAD_SIZE) CHECKSUM(1) STOP(2)\n
 * START and STOP escaped.\n
 * If escape character used in payload, its marked with an additional byte\n
 */
#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdint.h>
/** 
 * @brief
 * allowed to change, when serial_encode called puts into this buffer
 * @attention
 * actual size this_val - 1\n
 * must be bigger than single packet size, else packet always corrupted
 */
#define SERIAL_TX_BUF_SIZE 1000
/** 
 * @brief
 * serial_receives_byte puts into this buffer 
 * @attention
 * actual size this_val - 1
 */
#define SERIAL_RX_BUF_SIZE 1000
/** @brief payload pointer size in serial_payload_handler */
#define SERIAL_PAYLOAD_BUF_SIZE 1000
/** 
 * @brief PAYLOAD_ID(1) PAYLOAD_SIZE(1) PAYLOAD(PAYLOAD_SIZE) CHECKSUM(1) defined as content
 * @attention above must fit into buffer, adjust according to PAYLOAD_SIZE
 */
#define CONTENT_BUF_SIZE 1000

/**
 * @brief
 * first thing to do, inits tx rx buffers
 * @attention
 * rx tx buffers size user settable via macros 
 */
void serial_init(void);
/**
 * @brief packages data w/ given parameters and puts into tx buffer 
 */
void serial_encode(uint8_t payload_id, uint8_t payload_size, void *payload_ptr);
/** @brief when hardware received byte array, pass w/ this function to serial  */
void serial_receives_buf(uint8_t *buf, int len);
/** @brief when hardware received byte, pass w/ this function to serial  */
void serial_receives_byte(uint8_t byt);
/** 
 * @brief
 * put this into your main loop\n
 * handles received bytes in rx buffer\n
 * also sends bytes in tx buffer
 */
void serial_loop(void);
/**
 * @brief
 * hardware specific implementation to send single byte
 * @attention
 * user have to implement and for now only polling send supported(wait until byte goes)
 */
void serial_send_byte(uint8_t byt) __attribute__ ((weak));
/**
 * @brief
 * received bytes processed in serial loop, take your payload
 * @attention
 * when copy payload safe_memory_copy recommended
 */
void serial_payload_handler(uint8_t payload_id, uint8_t payload_size
			    , void *payload) __attribute__ ((weak));
/**
 * @brief
 * core may not allowed access 32 bit or 16 bit certain memory locations\n
 * uses byte access
 */
void safe_memory_copy(void *dest, void *src, uint8_t size);

#endif	/* __SERIAL_H */
