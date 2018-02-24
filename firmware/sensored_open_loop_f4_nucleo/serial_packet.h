/**
 * @file serial_packet.h
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
#ifndef __SERIAL_PACKET_H
#define __SERIAL_PACKET_H

#include <stdint.h>
/* @brief only change this values */
#define SERIAL_PACKET_MAX_PAYLOAD_SIZE 10
#define SERIAL_PACKET_MAX_PACKET_COUNT 3

/**
 * @brief
 * allowed to change, when serial_encode called puts into this buffer
 * @attention
 * actual size this_val - 1\n
 * must be bigger than single packet size, else packet always corrupted
 */
#define SERIAL_PACKET_TX_BUF_SIZE (((10 + SERIAL_PACKET_MAX_PAYLOAD_SIZE * 2) \
                                   * SERIAL_PACKET_MAX_PACKET_COUNT)+1)
/**
 * @brief
 * serial_receives_byte puts into this buffer
 * @attention
 * actual size this_val - 1
 */
#define SERIAL_PACKET_RX_BUF_SIZE (((10 + SERIAL_PACKET_MAX_PAYLOAD_SIZE * 2) \
                                   * SERIAL_PACKET_MAX_PACKET_COUNT)+1)
/** @brief payload pointer size in serial_payload_handler */
#define SERIAL_PACKET_PAYLOAD_BUF_SIZE (SERIAL_PACKET_MAX_PAYLOAD_SIZE)
/**
 * @brief PAYLOAD_ID(1) PAYLOAD_SIZE(1) PAYLOAD(PAYLOAD_SIZE) CHECKSUM(1) defined as content
 * @attention above must fit into buffer, adjust according to PAYLOAD_SIZE
 */
#define SERIAL_PACKET_CONTENT_BUF_SIZE (SERIAL_PACKET_MAX_PAYLOAD_SIZE + 3)

/**
 * @brief
 * first thing to do, inits tx rx buffers
 * @attention
 * rx tx buffers size user settable via macros
 */
int8_t serial_packet_init(void);
/**
 * @brief packages data w/ given parameters and puts into tx buffer
 */
void serial_packet_encode(uint8_t payload_id, uint8_t payload_size, void *payload_ptr);
/** @brief same as serial_encode, wait untils all data sent */
void serial_packet_encode_poll(uint8_t pid, uint8_t psize, void *pptr);

/** @brief when hardware received byte, pass w/ this function to serial packet */
void serial_packet_read(uint8_t byt);
/**
 * @brief
 * put this into your main loop\n
 * handles received bytes in rx buffer\n
 * also sends bytes in tx buffer
 */
void serial_packet_flush(void);
/**
 * @brief
 * hardware specific implementation to send single byte
 * @attention
 * user have to implement and for now only polling send supported(wait until byte goes)
 */
void serial_packet_print(uint8_t byt) __attribute__ ((weak));
/**
 * @brief
 * received bytes processed in serial loop, take your payload
 * @attention
 * when copy payload safe_memory_copy recommended
 */
void serial_packet_handler(uint8_t payload_id, uint8_t payload_size
			    , void *payload) __attribute__ ((weak));
/**
 * @brief
 * core may not allowed access 32 bit or 16 bit certain memory locations\n
 * uses byte access
 */
void safe_memory_copy(void *dest, void *src, uint8_t size);

#endif	/* __SERIAL_PACKET_H */
