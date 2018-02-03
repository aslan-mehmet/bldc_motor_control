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
#include "serial.h"
#include "fifo.h"

#define SERIAL_ESCAPE_BYTE ((uint8_t) 0x5c)
#define SERIAL_START ((uint8_t) 0x01)
#define SERIAL_STOP ((uint8_t) 0x02)
/* if serial_escape_byte occured in data content, state not mean to escaped */
#define SERIAL_NOT_ESCAPE ((uint8_t) 0x03)

static fifo _tx_fifo;
static uint8_t _tx_buf[SERIAL_TX_BUF_SIZE];

static fifo _rx_fifo;
static uint8_t _rx_buf[SERIAL_RX_BUF_SIZE];

static uint8_t _payload_id, _payload_size;
static uint8_t _payload[SERIAL_PAYLOAD_BUF_SIZE];

static uint8_t _content[CONTENT_BUF_SIZE];
static uint16_t _content_iteration = 0;
static uint8_t _content_started = 0;
static uint8_t _escape_byte_received = 0;

void serial_init(void)
{
	_tx_fifo.buf = _tx_buf;
	_tx_fifo.size = SERIAL_TX_BUF_SIZE;
	init_fifo(&_tx_fifo);

	_rx_fifo.buf = _rx_buf;
	_rx_fifo.size = SERIAL_RX_BUF_SIZE;
	init_fifo(&_rx_fifo);
}
/* in case data equals escape char in payload, mark not mean to be escaped */
static void data_write(uint8_t data)
{
	write_fifo(&_tx_fifo, data);
	
	if (data == SERIAL_ESCAPE_BYTE) {
		write_fifo(&_tx_fifo, SERIAL_NOT_ESCAPE);
	}
}

/* data written with escape char, have special meaning */
static void escaped_write(uint8_t data)
{
	write_fifo(&_tx_fifo, SERIAL_ESCAPE_BYTE);
	write_fifo(&_tx_fifo, data);
}

/* packages nicely, puts into tx buf */
void serial_encode(uint8_t pid, uint8_t psize, void *pptr)
{
        escaped_write(SERIAL_START);

	data_write(pid);

	data_write(psize);

	uint8_t *payload = (uint8_t *) pptr;
	uint8_t checksum = 0;
	
	for (uint16_t i = 0; i < psize; ++i) {
		checksum ^= payload[i];
		data_write(payload[i]);
	}

	data_write(checksum);

	escaped_write(SERIAL_STOP);
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
	
	serial_payload_handler(_payload_id, _payload_size, _payload);
}

/* i wont get segfault */
static void content_write(uint8_t byt)
{
	if (_content_iteration == CONTENT_BUF_SIZE) {
		return;
	}

	_content[_content_iteration++] = byt;
}

/* extracting escaped chars and processes and writes to content buffer */
static void process_received_byte(uint8_t byt)
{
	if (byt == SERIAL_ESCAPE_BYTE) {
		_escape_byte_received = 1;
		return;
	}

	if (_escape_byte_received) {

		switch (byt) {
		case SERIAL_START:
			_content_started = 1;
			_content_iteration = 0;
			break;
		case SERIAL_STOP:
			process_serial_content();
			_content_started = 0;
			_content_iteration = 0;
			break;
		case SERIAL_NOT_ESCAPE:
			if (_content_started) {
				content_write(SERIAL_ESCAPE_BYTE);
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


void serial_loop(void)
{
	int16_t hold_r = -1;
	int16_t hold_t = -1;

	do {
		if ((hold_t = read_fifo(&_tx_fifo)) != -1) {
			serial_send_byte((uint8_t) hold_t);
		}

		if ((hold_r = read_fifo(&_rx_fifo)) != -1) {
			process_received_byte((uint8_t) hold_r);
		}
	} while (hold_r != -1 || hold_t != -1);

}

void serial_receives_buf(uint8_t *buf, int len)
{
        for (int i = 0; i < len; ++i) {
                serial_receives_byte(buf[i]);
        }
}

void serial_receives_byte(uint8_t byt)
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

