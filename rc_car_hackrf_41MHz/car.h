#ifndef __CAR_H_
#define __CAR_H_

#include <stdint.h>

#define PULSE_LENGTH_US 300

/* Signal format: 4x Big pulse, count x small pulse
     - each pulse is followed by silence of same duration
     - big pulse is 3x the duration of a small pulse(300us)
     - count depends on the direction
*/

#define FORWARD_PULSES 10
#define FORWARD_LEFT_PULSES 28
#define FORWARD_RIGHT_PULSES 34

#define REVERSE_PULSES 40
#define REVERSE_LEFT_PULSES 52
#define REVERSE_RIGHT_PULSES 46

#define LEFT_PULSES 58
#define RIGHT_PULSES 64

enum car_direction
{
    fwd,
    fwd_left,
    fwd_right,
    stop,
    rev,
    rev_left,
    rev_right,
    left,
    right
};

typedef enum car_direction car_direction_t;

unsigned short calculate_buffer_size(car_direction_t dir);

extern const uint8_t pattern[];
extern const unsigned int preamble_length;

#endif // __CAR_H_
