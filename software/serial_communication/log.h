/**
 * @file log.h
 * @author Mehmet ASLAN
 * @date December 3, 2017
 * @copyright Gnu General Public License Version 3 or Later
 * @brief creates serial_rx_log_bin, does the thing
 */
#ifndef __LOG_H
#define __LOG_H

#include <stdint.h>
/** @brief can close while running */
void close_log(void);
void exit_log(void);
/** @brief open log files with append */
void start_log(void);
/** @brief if logging not started nothing will happen */
void loop_log(uint8_t *ptr, int len);

#endif /* __LOG_H */
