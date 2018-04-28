/**
 * @file fifo.h
 * @author Mehmet ASLAN
 * @date November 12, 2017
 * @copyright Gnu General Public License Version 3 or Later
 * @brief 
 * fifo implementation with circular buffer\n
 * data thought as snake which moves in buffer\n
 * snake cant eat its tail\n
 * but length of snake can be zero that means there is no data
 * @bug
 * when multiple threads read and write at the same time dont know what happen
 */

#ifndef __FIFO_H
#define __FIFO_H

#include <stdint.h>
/**
 * @brief
 * fifo data type for internal operations\n
 * allows to create multiple fifos in same project
 * @attention
 * buf pointer and its size must be set by user\n
 * before doing any operation
 */
typedef struct{
	/**
	 * @brief pointer to staticly created buffer 
	 * @attention
	 * buf pointer and its size must be set by user\n
	 * before doing any operation
	 */
	uint8_t *buf;
	/**
	 * @brief 
	 * buffer size\n
	 * actual length of the fifo\n
	 * len = size -1\n
	 * like null terminated string\n
	 * why ? --> cause head index always empty
	 * @attention
	 * buf pointer and its size must be set by user\n
	 * before doing any operation
	 */
	uint16_t size;
	/** @brief dont care about this as user */
	uint16_t head, tail;
}fifo;

/** 
 * @brief do fifo data type obligations then call first
 * @attention look fifo data type, set manually
 */
void init_fifo(fifo *f);
/**
 * @retval -1 if full. data not written 
 * @retval 0 success
 */
int8_t write_fifo(fifo *f, uint8_t data);
/**
 * @retval -1 if empty
 * @retval uint8_t data read
 */
int16_t read_fifo(fifo *f);
/** @brief empty all data in fifo */
void flush_fifo(fifo *f);

#endif	/* __FIFO_H */
