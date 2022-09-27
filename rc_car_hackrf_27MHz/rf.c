#include "rf.h"
#include "car.h"

#include <stdio.h>
#include <string.h>
#include <pthread.h>

static hackrf_device *device = NULL;

volatile car_direction_t dir = STOP;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


#define PULSE_LENGTH_US 550
#define PAUSE_DURATION_US 10200

/*
 * Remote control of the 27MHz super blitzer toy car
 * Signal format: 21 bits, each symbol is 550us
 * 1 is high, 0 is low
 * gap of 10.2ms between frames
 * Bit patterns, least significant bit first (right)
 */

#define FRAME_BITS 21

const uint32_t FORWARD_PULSES         = 0x171111; //100010001000100011101
const uint32_t FORWARD_LEFT_PULSES    = 0x171171; //100011101000100011101
const uint32_t FORWARD_RIGHT_PULSES   = 0x171711; //100010001110100011101
const uint32_t REVERSE_PULSES         = 0x117111; //100010001000111010001
const uint32_t REVERSE_LEFT_PULSES    = 0x117171; //100011101000111010001
const uint32_t REVERSE_RIGHT_PULSES   = 0x117711; //100010001110111010001
const uint32_t LEFT_PULSES            = 0x111171; //100011101000100010001
const uint32_t RIGHT_PULSES           = 0x111711; //100010001110100010001


static int tx_callback(hackrf_transfer* transfer);

int init_hackrf(void)
{
    int result = hackrf_init();
    if( result != HACKRF_SUCCESS )
    {
        return result;
    }

    hackrf_device_list_t *list = hackrf_device_list();

    if (list->devicecount < 1 )
    {
		printf("No HackRF boards found.\n");
		return -1;
    }

    printf("found hackrf device\n");

    result = hackrf_device_list_open(list, 0, &device); //open first device
    if (result != HACKRF_SUCCESS)
    {
        return result;
    }
    printf("hackrf serial: %s\n", list->serial_numbers[0]);
    printf("hackrf USB device: %i\n", list->usb_device_index[0]);

    hackrf_device_list_free(list);

    result = configure_hackrf(sample_rate_hz, freq_hz, txvga_gain);

    printf("hackrf sample rate: %lluHz\n", sample_rate_hz);
    printf("hackrf TX gain: %idB\n", txvga_gain);
    printf("hackrf TX frequency: %lluHz\n", freq_hz);


    return result;

}

void shutdown_hackrf(void)
{
    hackrf_stop_tx(device);
    hackrf_close(device);

    printf("hackrf device shutdown\n");

    hackrf_exit();
}

int configure_hackrf(unsigned long long sample_rate, unsigned long long frequency, unsigned int gain)
{
    int result = hackrf_set_sample_rate(device, sample_rate);
    if( result != HACKRF_SUCCESS )
    {
        return result;
    }

    result = hackrf_set_txvga_gain(device, gain);
    if( result != HACKRF_SUCCESS )
    {
        return result;
    }

    result = hackrf_set_freq(device, frequency);
	if( result != HACKRF_SUCCESS )
    {
        return result;
    }

    result = hackrf_start_tx(device, tx_callback, NULL);
    if( result != HACKRF_SUCCESS )
    {
        return result;
    }

    return 0;
}

static int tx_callback(hackrf_transfer* transfer)
{
    /* Total duration of a frame:
        21 x 0.55ms (frame) + 10.2ms (break) = 21.75ms
        @ 2 Msamples / sec, exactly 87000 samples are needed
    */

    const unsigned int samples_per_symbol = PULSE_LENGTH_US * (sample_rate_hz / 1000000L) * 2;
    const unsigned int pause_samples = PAUSE_DURATION_US * (sample_rate_hz / 1000000L) * 2;
    const unsigned int total_samples = samples_per_symbol * FRAME_BITS + pause_samples;

    car_direction_t tmp_dir = STOP;

    pthread_mutex_lock(&mutex);
    tmp_dir = dir;
    pthread_mutex_unlock(&mutex);


    uint32_t bits = 0;
    switch (tmp_dir)
    {
    case FWD:
        bits = FORWARD_PULSES;
        break;
    case FWD_LEFT:
        bits = FORWARD_LEFT_PULSES;
        break;
    case FWD_RIGHT:
        bits = FORWARD_RIGHT_PULSES;
        break;
    case REV:
        bits = REVERSE_PULSES;
        break;
    case REV_LEFT:
        bits = REVERSE_LEFT_PULSES;
        break;
    case REV_RIGHT:
        bits = REVERSE_RIGHT_PULSES;
        break;
    case LEFT:
        bits = LEFT_PULSES;
        break;
    case RIGHT:
        bits = RIGHT_PULSES;
        break;
    case STOP:
    default:
        memset(transfer->buffer, 0, transfer->valid_length);
        return 0;
    }

    uint8_t * buffer = transfer->buffer;

    const unsigned int repeats = transfer->valid_length / total_samples;
    for (unsigned int j = 0; j < repeats; j++)
    {
        for(int i = 0; i < FRAME_BITS; i++)
        {
            if (bits & (1 << i))
            {
                memset(buffer, SAMPLE, samples_per_symbol);
                buffer += samples_per_symbol;
            }
            else
            {
                memset(buffer, 0, samples_per_symbol);
                buffer += samples_per_symbol;
            }
        }
        memset(buffer, 0, pause_samples);
        buffer += pause_samples;
    }

    //TODO: there might be a few remaining bytes
    memset(buffer, 0, transfer->valid_length - (buffer - transfer->buffer));

    return 0;
}


void set_direction(car_direction_t new_dir)
{
    car_direction_t tmp_dir = STOP;

    pthread_mutex_lock(&mutex);
    tmp_dir = dir;
    pthread_mutex_unlock(&mutex);


    if (new_dir != tmp_dir)
    {
        pthread_mutex_lock(&mutex);
        dir = new_dir;
        pthread_mutex_unlock(&mutex);
    }
}
