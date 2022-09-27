#ifndef RF_H_INCLUDED
#define RF_H_INCLUDED

#include <libhackrf/hackrf.h>
#include "car.h"

static const unsigned int txvga_gain = 45;
static const long long sample_rate_hz = 2000000LL;
static const long long freq_hz = 40686000LL;

int init_hackrf(void);
void shutdown_hackrf(void);

int configure_hackrf(unsigned long long sample_rate, unsigned long long frequency, unsigned int gain);

void set_direction(car_direction_t new_dir);

#endif // RF_H_INCLUDED
