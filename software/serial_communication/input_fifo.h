#ifndef __SERVER_H
#define __SERVER_H

int input_fifo_open(void);
void input_fifo_process(void);
void input_fifo_close(void);

#endif
