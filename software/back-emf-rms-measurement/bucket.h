#ifndef __BUCKET_H
#define __BUCKET_H

#define BUCKET_DATA_LEN 10000
#include <stdint.h>

struct bucket {
        int id;
        double data[BUCKET_DATA_LEN];
        int index;
};

void bucket_init(struct bucket *bucket_ptr, int id);
/**
 * every time bucket is full
 * handler called->processed->bucket emptied
 * void bucket_full_handler(struct bucket *bucket_ptr);
 */
void bucket_put_data(struct bucket *bucket_ptr, double data);
void bucket_put_data_buffer(struct bucket *bucket_ptr, double *data, int data_len);

#endif /* __BUCKET_H */
