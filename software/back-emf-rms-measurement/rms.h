#ifndef __RMS_H
#define __RMS_N

#include <stdint.h>

void calculate_rms(double *signal, int signal_length, double trigger_point);
double get_last_calculated_rms_val(void);

#endif /* __RMS_H */
