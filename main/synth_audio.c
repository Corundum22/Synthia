#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "driver/dac_continuous.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "portmacro.h"
#include "waveform_tables.h"
#include "uart_handler.h"
#include "synth_audio.h"
#include "note_handler.h"
#include "global_header.h"
#include "sequencer.h"
#include "data_y_splitter.h"
#include "basic_io.h"
#include <string.h>


#define NUM_COMB_VOICES (NUM_VOICES + SEQ_VOICES)

#define GET_NOTE(i) \
if (note_properties[i].is_sounding == true) {\
    data += wave(note_properties[i].note_num, note_properties[i].multiplier, time);\
}



dac_continuous_handle_t dac_handle;

uint_fast16_t *current_wave1 = sin_array;
uint_fast16_t *current_wave2 = sin_array;

// Defines how many times the time variable should increment before old_samples_index increments
#define OLD_QUALITY_SAMPLE_TIME_FACTOR 1
// OLD_SAMPLES_SIZE must be a multiple of 2
#define OLD_SAMPLES_SIZE 8192
uint16_t old_samples[OLD_SAMPLES_SIZE] = { 0 };
uint16_t old_samples_index = 0;


static inline uint16_t get_old_samples(uint32_t samples_ago) {
    return old_samples[(old_samples_index - samples_ago) & 0b1111111111111];
}

static inline uint32_t wave(uint_fast8_t midi_note_number, uint_fast16_t multiply_val, uint_fast32_t time) {
    uint_fast32_t ratio_numerator = ratio_num[midi_note_number];
    uint_fast32_t ratio_denominator = ratio_denom[midi_note_number];

    uint_fast32_t point_in_cycle = ((time * ratio_numerator) / ratio_denominator) & 0b11111111;

    uint32_t result = (current_wave1[point_in_cycle] * wave_blend_syn) + (current_wave2[point_in_cycle] * wave_blend_pair_syn);
    result = (result * multiply_val) >> (MULTIPLIER_WIDTH + BLEND_VAL_MAX_BITS);
    return result;
}

static inline uint16_t audio_sample_get(uint32_t time) {
    uint32_t data = 0;

    #if NUM_COMB_VOICES >= 1
        GET_NOTE(0)
    #endif
    #if NUM_COMB_VOICES >= 2
        GET_NOTE(1)
    #endif
    #if NUM_COMB_VOICES >= 3
        GET_NOTE(2)
    #endif
    #if NUM_COMB_VOICES >= 4
        GET_NOTE(3)
    #endif
    #if NUM_COMB_VOICES >= 5
        GET_NOTE(4)
    #endif
    #if NUM_COMB_VOICES >= 6
        GET_NOTE(5)
    #endif
    #if NUM_COMB_VOICES >= 7
        GET_NOTE(6)
    #endif
    #if NUM_COMB_VOICES >= 8
        GET_NOTE(7)
    #endif
    #if NUM_COMB_VOICES >= 9
        GET_NOTE(8)
    #endif
    #if NUM_COMB_VOICES >= 10
        GET_NOTE(9)
    #endif

    data /= (NUM_VOICES + SEQ_VOICES);

    // This should introduce a delay of 6000 * 16 / 44100 seconds
    data += get_old_samples(6000) / 2; // TODO: average with neighboring samples to prevent high freq artifacts due to alias of what are originally even higher freq components
    data += get_old_samples(6002) / 3;
    data += get_old_samples(6004) / 4;
    data += get_old_samples(6006) / 5;
    data += get_old_samples(5998) / 3;
    data += get_old_samples(5996) / 4;
    data += get_old_samples(5994) / 5;

    return data;
}

static inline void set_current_wave() {
    switch (wave_select1_syn) {
        case ssin:
            current_wave1 = sin_array;
            break;
        case striangle:
            current_wave1 = triangle_array;
            break;
        case ssawtooth:
            current_wave1 = sawtooth_array;
            break;
        case ssquare:
            current_wave1 = square_array;
            break;
        default:
            break;
    }

    switch (wave_select2_syn) {
        case ssin:
            current_wave2 = sin_array;
            break;
        case striangle:
            current_wave2 = triangle_array;
            break;
        case ssawtooth:
            current_wave2 = sawtooth_array;
            break;
        case ssquare:
            current_wave2 = square_array;
            break;
        default:
            break;
    }
}

void task_audio_generate() {

    uint32_t time = 0;

    while (1) {

        // Set the current wavetable
        set_current_wave();
 
        uint8_t data_array[AUDIO_BUF_SIZE * NUM_DAC_CHANNELS];

        for (uint_fast16_t i = 0; i < AUDIO_BUF_SIZE * NUM_DAC_CHANNELS; i += 2) {
            uint16_t data = audio_sample_get(time);
            
            data_array[i] = (uint8_t) data;
            data_array[i + 1] = (uint8_t) (data >> 8);

            old_samples[old_samples_index] = data;
            // This condition is true when i % OLD_QUALITY_REDUCTION_FACTOR == 0
            if (i & OLD_QUALITY_SAMPLE_TIME_FACTOR) old_samples_index++;
            if (old_samples_index >= OLD_SAMPLES_SIZE) old_samples_index = 0;
        
            time++;
        }

        // Advance each note's envelope
        envelope_advance();

        ESP_ERROR_CHECK(dac_continuous_write(dac_handle, data_array, AUDIO_BUF_SIZE * NUM_DAC_CHANNELS, NULL, -1));
   

    }
}

void dac_init() {
    // Configure DAC for lower 8 bits of output signal
    dac_continuous_config_t dac_config = {
        .chan_mask = DAC_CHANNEL_MASK_ALL,
        .desc_num = 16,
        .buf_size = AUDIO_BUF_SIZE * NUM_DAC_CHANNELS,
        .freq_hz = OUTPUT_SAMPLE_RATE,
        .offset = 0,
        .clk_src = DAC_DIGI_CLK_SRC_APLL,
        .chan_mode = DAC_CHANNEL_MODE_ALTER,
    };
    ESP_ERROR_CHECK(dac_continuous_new_channels(&dac_config, &dac_handle));


    ESP_ERROR_CHECK(dac_continuous_enable(dac_handle));
}
