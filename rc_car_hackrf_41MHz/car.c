#include "car.h"

/* the bit pattern of the signal (raw binary, no symbols created yet)
    This array is for the longest pattern, each direction is a subset of this.
*/
const uint8_t pattern[] =
{
    1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0, // preamble
    1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0, // data bits 0 to 15
    1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0, // data bits 16 to 31
    1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0, // data bits 32 to 47
    1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0, // data bits 48 to 63
    1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0, // data bits 64 to 79
    1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0, // data bits 80 to 95
    1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0, // data bits 96 to 111
    1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0 // data bits 112 to 127
};

const unsigned int preamble_length = 16;


unsigned short calculate_buffer_size(car_direction_t dir)
{
    //const unsigned int samples_per_symbol = PULSE_LENGTH_US * (sample_rate / 1000000L);

    unsigned short pulses = 0;

    switch (dir)
    {
    case fwd:
        pulses = FORWARD_PULSES;
        break;
    case fwd_left:
        pulses = FORWARD_LEFT_PULSES;
        break;
    case fwd_right:
        pulses = FORWARD_RIGHT_PULSES;
        break;
    case rev:
        pulses = REVERSE_PULSES;
        break;
    case rev_left:
        pulses = REVERSE_LEFT_PULSES;
        break;
    case rev_right:
        pulses = REVERSE_RIGHT_PULSES;
        break;
    case left:
        pulses = LEFT_PULSES;
        break;
    case right:
        pulses = RIGHT_PULSES;
        break;
    default:
        return 0;
        break;
    }

    pulses *= 2;

    return pulses;
}
