#include "rf.h"

#include <stdio.h>
#include <string.h>
#include <pthread.h>

static hackrf_device *device = NULL;

volatile car_direction_t dir = stop;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#ifdef STOP_TX
static volatile int running = 0;
#endif

// keep track of last position in data arrays
static volatile unsigned int pattern_index = 0;


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
    int result = hackrf_stop_tx(device);
    if (result != HACKRF_SUCCESS)
    {

    }

    result = hackrf_close(device);
    if (result != HACKRF_SUCCESS)
    {
    }

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

    #ifndef STOP_TX
    result = hackrf_start_tx(device, tx_callback, NULL);
    if( result != HACKRF_SUCCESS )
    {
        return result;
    }
    #endif // STOP_TX

    return 0;

}

static int tx_callback(hackrf_transfer* transfer)
{
    /* The transfer->buffer array is interleaved 8bit I and Q samples.
        In order to perform On Off Keying, each sample is a complex number of magnitude 1
        re(sample) = cos theta, im(sample) = sin theta
        theta = pi/4 so that (cos pi/4)^2 + (sin pi/4)^2 = 1
        each complex component is a float so scale to 0-127 to fit in the integer.

        cos(pi/4) = sin(pi/4) = 1/sqrt(2) = 0.707...

        re[n] = round(127*0.707...) im[n] = round(127*0.707...) = 89.9....

        transfer->valid_length = 262144
    */


    car_direction_t tmp_dir;
    unsigned int data_pos;

    pthread_mutex_lock(&mutex);
    tmp_dir = dir;
    data_pos = pattern_index;
    pthread_mutex_unlock(&mutex);

    const unsigned short symbols = calculate_buffer_size(tmp_dir);

    const unsigned int samples_per_symbol = PULSE_LENGTH_US * (sample_rate_hz / 1000000L) * 2;
    const uint8_t sample = 89;

    unsigned int bytes_written = 0;

    uint8_t * buffer = transfer->buffer;

    memset(buffer, 0, transfer->valid_length);

    #ifndef STOP_TX
    if (tmp_dir == stop)
    {
        pthread_mutex_lock(&mutex);
        pattern_index = 0;
        pthread_mutex_unlock(&mutex);
        return 0;
    }
    #endif // STOP_TX


    while (bytes_written + samples_per_symbol <= transfer->valid_length) //while there is enough bytes left to write to
    {
        memset(buffer, sample * pattern[data_pos], samples_per_symbol); // create symbols from the bits
        bytes_written += samples_per_symbol;

        data_pos++;

        buffer = transfer->buffer + bytes_written; //move the pointer to the next empty byte

        if (data_pos == preamble_length + symbols)
        {
            data_pos = 0;
        }
    }

    //TODO: there might be a few remaining bytes

    pthread_mutex_lock(&mutex);
    pattern_index = data_pos;
    pthread_mutex_unlock(&mutex);

    return 0;
}


void set_direction(car_direction_t new_dir)
{
    car_direction_t tmp_dir;

    pthread_mutex_lock(&mutex);
    tmp_dir = dir;
    pthread_mutex_unlock(&mutex);


    if (new_dir != tmp_dir)
    {
        #ifdef STOP_TX

        pattern_index = 0;

        if (new_dir == stop && running)
        {
            hackrf_stop_tx(device); //do not transmit if in stop state
            running = 0;
        }
        else
        {
            if (!running)
            {
                configure_hackrf(sample_rate_hz, freq_hz, txvga_gain);
                hackrf_start_tx(device, tx_callback, NULL);
                running = 1;
            }
        }
        #endif // STOP_TX

        pthread_mutex_lock(&mutex);
        dir = new_dir;
        pattern_index = 0;
        pthread_mutex_unlock(&mutex);
    }
}
