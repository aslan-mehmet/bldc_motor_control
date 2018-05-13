#include "rms.h"
#include <math.h>
#include <stdio.h>

double _last_calculated_rms_val = 0;

void use_rms_value(double rms)
{
}

double get_last_calculated_rms_val(void)
{
        return _last_calculated_rms_val;
}

void calculate_rms(double *signal, int signal_length, double trigger_point)
{
        double sum = 0;
        int period = 0;

        for (int i = 1; i < signal_length; ++i) {
                /* rising edge condition */
                if (signal[i-1] < trigger_point && signal[i] >= trigger_point) {
                        /* in case it happens */
                        if (period != 0) {
                                double mean = sum / period;
                                double rms = sqrt(mean);
                                _last_calculated_rms_val = rms;
                                use_rms_value(rms);
                        }

                        /* new wave period start */
                        sum = 0;
                        period = 0;
                }

                /* square the signal */
                sum += signal[i] * signal[i];
                ++period;
        }
}
