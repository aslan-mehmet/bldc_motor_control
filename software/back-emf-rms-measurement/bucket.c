#include "bucket.h"
#include "rms.h"

void bucket_init(struct bucket *bucket_ptr, int id)
{
        bucket_ptr->id = id;
        for (int i = 0; i < BUCKET_DATA_LEN; ++i) {
                bucket_ptr->data[i] = 0;
        }
        bucket_ptr->index = 0;
}

void bucket_full_handler(struct bucket *bucket_ptr)
{
        calculate_rms(bucket_ptr->data, BUCKET_DATA_LEN, 125);
        bucket_ptr->index = 0;
}

void bucket_put_data(struct bucket *bucket_ptr, double data)
{
        if (bucket_ptr->index == BUCKET_DATA_LEN) {
                bucket_full_handler(bucket_ptr);
        }

        bucket_ptr->data[bucket_ptr->index++] = data;
}

void bucket_put_data_buffer(struct bucket *bucket_ptr, double *data, int data_len)
{
        for (int i = 0; i < data_len; ++i) {
                /* bucket full */
                if (bucket_ptr->index == BUCKET_DATA_LEN) {
                        bucket_full_handler(bucket_ptr);
                }

                bucket_ptr->data[bucket_ptr->index++] = data[i];
        }
}
