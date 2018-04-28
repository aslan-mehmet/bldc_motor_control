/**
 * @file serial.c
 * @author Mehmet ASLAN
 * @date November 19, 2017
 * @copyright Gnu General Public License Version 3 or Later
 * @brief
 * this is a serial communication protocol\n
 * How to Use\n
 * set buffer size macros\n
 * init w/ serial_init\n
 * implement weak functions in header file\n
 * put serial_loop into main loop\n
 * send and receive via encode and handler\n
 * done
 */
#include "serial_packet.h"
#include "fifo.h"

#define SERIAL_PACKET_ESCAPE_BYTE ((uint8_t) 0x5c)
#define SERIAL_PACKET_START ((uint8_t) 0x01)
#define SERIAL_PACKET_STOP ((uint8_t) 0x02)
/* if serial_escape_byte occured in data content, state not mean to escaped */
#define SERIAL_PACKET_NOT_ESCAPE ((uint8_t) 0x03)

static fifo _tx_fifo;
static uint8_t _tx_buf[SERIAL_PACKET_TX_BUF_SIZE];

static fifo _rx_fifo;
static uint8_t _rx_buf[SERIAL_PACKET_RX_BUF_SIZE];

static uint8_t _payload_id, _payload_size;
static uint8_t _payload[SERIAL_PACKET_PAYLOAD_BUF_SIZE];

static uint8_t _content[SERIAL_PACKET_CONTENT_BUF_SIZE];
static uint16_t _content_iteration = 0;
static uint8_t _content_started = 0;
static uint8_t _escape_byte_received = 0;

static int8_t sys_check(void)
{
	/* check */
	/* size */
	uint32_t u32;
	float f;
	int32_t i32;
	uint8_t *p = (uint8_t *) &u32;

        if (sizeof(u32) != 4) { /* something really wrong */
		return -1;
	}

	if (sizeof(f) != 4) {
		return -2;
	}


	/* 2's complement */
       	u32 = 0xffffff9c;

	i32 = -100;
	uint8_t *t = (uint8_t *) &i32;
	for (int i = 0; i < 4; ++i) {
		if (p[i] != t[i]) {
			return -3;
		}
	}

	/* little endian */
	u32 = 0x12345678;
	uint8_t little[4] = {0x78, 0x56, 0x34, 0x12};
	for (int i = 0; i < 4; ++i) {
		if (little[i] != p[i]) {
			return -4;
		}
	}

	/* decimal number representations */
	f = -248.75;
	u32 = 0xc378c000; /* ieee 754 std */
	uint8_t *fp = (uint8_t *) &f;

	for (int i = 0; i < 4; ++i) {
		if (fp[i] != p[i]) {
			return -5;
		}
	}

	/* also memory allows byte access */
	return 0;
}

int8_t serial_packet_init(void)
{
	_tx_fifo.buf = _tx_buf;
	_tx_fifo.size = SERIAL_PACKET_TX_BUF_SIZE;
	init_fifo(&_tx_fifo);

	_rx_fifo.buf = _rx_buf;
	_rx_fifo.size = SERIAL_PACKET_RX_BUF_SIZE;
	init_fifo(&_rx_fifo);

        return sys_check();
}
/* in case data equals escape char in payload, mark not mean to be escaped */
static void data_write(uint8_t data)
{
	write_fifo(&_tx_fifo, data);

	if (data == SERIAL_PACKET_ESCAPE_BYTE) {
		write_fifo(&_tx_fifo, SERIAL_PACKET_NOT_ESCAPE);
	}
}

/* data written with escape char, have special meaning */
static void escaped_write(uint8_t data)
{
	write_fifo(&_tx_fifo, SERIAL_PACKET_ESCAPE_BYTE);
	write_fifo(&_tx_fifo, data);
}

/* packages nicely, puts into tx buf */
void serial_packet_encode(uint8_t pid, uint8_t psize, void *pptr)
{
        escaped_write(SERIAL_PACKET_START);

	data_write(pid);

	data_write(psize);

	uint8_t *payload = (uint8_t *) pptr;
	uint8_t checksum = 0;

	for (uint16_t i = 0; i < psize; ++i) {
		checksum ^= payload[i];
		data_write(payload[i]);
	}

	data_write(checksum);

	escaped_write(SERIAL_PACKET_STOP);
}

static void serial_packet_flush_tx_buf(void)
{
	int16_t hold = -1;

        while((hold = read_fifo(&_tx_fifo)) != -1) {
                serial_packet_print((uint8_t) hold);
        }
}

void serial_packet_encode_poll(uint8_t pid, uint8_t psize, void *pptr)
{
        serial_packet_encode(pid, psize, pptr);
        serial_packet_flush_tx_buf();
}
/*
 * at this point all escaped chars, start stop removed
 * only content exist, pieces identified with their locations
 */
static void process_serial_content(void)
{
        _payload_id = _content[0];
	_payload_size = _content[1];

	/* check content size what is suppose to be */
	if (_content_iteration != _payload_size + 3) {
		return;
	}

	safe_memory_copy(_payload, _content + 2, _payload_size);

	uint8_t checksum = 0;

	for (uint8_t i = 0; i < _payload_size; ++i) {
		checksum ^= _payload[i];
	}

	if (checksum != _content[_content_iteration - 1]) {
		return;
	}

	serial_packet_handler(_payload_id, _payload_size, _payload);
}

/* i wont get segfault */
static void content_write(uint8_t byt)
{
	if (_content_iteration == SERIAL_PACKET_CONTENT_BUF_SIZE) {
		return;
	}

	_content[_content_iteration++] = byt;
}

/* extracting escaped chars and processes and writes to content buffer */
static void process_received_byte(uint8_t byt)
{
	if (byt == SERIAL_PACKET_ESCAPE_BYTE) {
		_escape_byte_received = 1;
		return;
	}

	if (_escape_byte_received) {

		switch (byt) {
		case SERIAL_PACKET_START:
			_content_started = 1;
			_content_iteration = 0;
			break;
		case SERIAL_PACKET_STOP:
			process_serial_content();
			_content_started = 0;
			_content_iteration = 0;
			break;
		case SERIAL_PACKET_NOT_ESCAPE:
			if (_content_started) {
				content_write(SERIAL_PACKET_ESCAPE_BYTE);
			}
			break;
		}

		_escape_byte_received = 0;
		return;
	}

	if (_content_started) {
		content_write(byt);
	}
}

static void serial_packet_flush_rx_buf(void)
{
        int16_t hold = -1;

        while((hold = read_fifo(&_rx_fifo)) != -1) {
		process_received_byte((uint8_t) hold);
        }
}

void serial_packet_flush(void)
{
        serial_packet_flush_tx_buf();
        serial_packet_flush_rx_buf();
}

void serial_packet_read(uint8_t byt)
{
	write_fifo(&_rx_fifo, byt);
}

/* byte access memory copy */
void safe_memory_copy(void *dest, void *src, uint8_t size)
{
	uint8_t *d, *s;
	d = (uint8_t *) dest;
	s = (uint8_t *) src;

	for (int i = 0; i < size; ++i) {
		d[i] = s[i];
	}
}

