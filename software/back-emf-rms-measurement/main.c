#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <time.h>
#include "bucket.h"
#include "rs232.h"
#include "rms.h"

#define FRAME_HEADER_LEN 2
uint8_t _frame_header[FRAME_HEADER_LEN] = {0xaa, 0xbb};

#define FRAME_LEN 6
uint8_t _frame_buffer[FRAME_LEN];
#define _adc_bemfs_readings ((uint8_t *) (_frame_buffer+2))
#define _hall_current_step  ((uint8_t *) (_frame_buffer+5))

struct bucket _signal_data_bucket;

uint8_t is_frame_header(void)
{
        uint8_t it_is_header = 1;

        for (int i = 0; i < FRAME_HEADER_LEN; ++i) {
                if (_frame_buffer[i] != _frame_header[i]) {
                        it_is_header = 0;
                        break;
                }
        }

        return it_is_header;
}

void process_received_payload(void)
{
        uint8_t bemf;
        switch (*_hall_current_step) {
        case 5:
                bemf = _adc_bemfs_readings[2];
                break;
        case 4:
                bemf = _adc_bemfs_readings[1];
                break;
        case 6:
                bemf = _adc_bemfs_readings[0];
                break;
        case 2:
                bemf = _adc_bemfs_readings[2];
                break;
        case 3:
                bemf = _adc_bemfs_readings[1];
                break;
        case 1:
                bemf = _adc_bemfs_readings[0];
        }

        bucket_put_data(&_signal_data_bucket, (double)bemf);
}

void put_data_to_frame_buffer(uint8_t data)
{
        for (int i = 0; i < FRAME_LEN - 1; ++i) {
                _frame_buffer[i] = _frame_buffer[i+1];
        }

        _frame_buffer[FRAME_LEN-1] = data;

        if (is_frame_header()) {
                process_received_payload();
        }
}

void put_buffer_to_frame_buffer(uint8_t *data, int data_len)
{
        for (int i = 0; i < data_len; ++i) {
                put_data_to_frame_buffer(data[i]);
        }
}

#ifdef EXAMPLE_CODE
int main(void)
{
        FILE *fp = NULL;
        fp = fopen("bemf500.bin", "rb");

        if (fp == NULL) {
                puts("fail file ops");
                return -1;
        }

        #define BUFFER_LEN (1024)
        uint8_t buffer[BUFFER_LEN];
        int number_of_reads = 0;

        bucket_init(&_signal_data_bucket, 1);

        while ((number_of_reads = fread(buffer, sizeof(uint8_t), BUFFER_LEN, fp)) > 0) {
                put_buffer_to_frame_buffer(buffer, number_of_reads);
        }

        fclose(fp);
        return 0;
}
#endif

int _rs232_port_number = -1;
#define RS232_PORT_MODE_DATABITS_INDEX 0
#define RS232_PORT_MODE_PARITY_INDEX 1
#define RS232_PORT_MODE_STOP_BITS_INDEX 2
#define RS232_PORT_MODE_DONT_USE_INDEX 3
char _rs232_port_mode[4] = {'8', 'N', '1', 0};
int _rs232_port_baudrate = -1;
#define RS232_PORT_STATE_OPEN 1
#define RS232_PORT_STATE_CLOSE 0
int _rs232_port_state = RS232_PORT_STATE_CLOSE;

static void signal_handler(int signum)
{
	if (signum == SIGINT) {
                if (_rs232_port_state == RS232_PORT_STATE_OPEN) {
                        RS232_CloseComport(_rs232_port_number);
                }

                exit(0);
	}
}

int main(void)
{
        signal(SIGINT, signal_handler);

        _rs232_port_baudrate = 1000000;
        _rs232_port_number = RS232_GetPortnr("ttyUSB0");
        _rs232_port_state = RS232_PORT_STATE_OPEN;

        if (RS232_OpenComport(_rs232_port_number, _rs232_port_baudrate, _rs232_port_mode)) {
                puts("fail: rs232 port open");
                _rs232_port_state = RS232_PORT_STATE_CLOSE;
        } else {
                puts("success: rs232 port open");
        }

        int rs232_number_of_read_bytes = 0;
        #define RS232_INPUT_BUFFER_LEN 10000
        unsigned char rs232_input_buffer[RS232_INPUT_BUFFER_LEN];
        struct timespec result;
        uint64_t current_time_in_ms;
        clock_gettime(CLOCK_MONOTONIC_RAW, &result);
        uint64_t hold_time_in_ms = result.tv_sec * 1000 + result.tv_nsec / 1e6;

        while (1) {
                if (_rs232_port_state == RS232_PORT_STATE_OPEN) {
                        rs232_number_of_read_bytes = RS232_PollComport(_rs232_port_number, rs232_input_buffer, RS232_INPUT_BUFFER_LEN);
                }

                if (rs232_number_of_read_bytes >= 0) {
                        put_buffer_to_frame_buffer(rs232_input_buffer, rs232_number_of_read_bytes);
                }

                clock_gettime(CLOCK_MONOTONIC_RAW, &result);
                current_time_in_ms = result.tv_sec * 1000 + result.tv_nsec / 1e6;
                if (current_time_in_ms - hold_time_in_ms > 100) {
                        hold_time_in_ms = current_time_in_ms;

                        /* do whatever you want */
                        printf("%f\n", get_last_calculated_rms_val());
                }

        }
}
