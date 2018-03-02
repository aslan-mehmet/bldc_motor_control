#ifndef __SERVER_H
#define __SERVER_H

/* creates file named input_fifo accepts strings */
int input_fifo_open(void);
/* received strings turn into command */
void input_fifo_process(void);
/* file closed and used memory released */
void input_fifo_close(void);

#endif
