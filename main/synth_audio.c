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


// Get value for single voice
static inline uint32_t wave(uint_fast8_t midi_note_number, uint_fast16_t multiply_val, uint_fast32_t time) {
    uint_fast32_t ratio_numerator = ratio_num[midi_note_number];
    uint_fast32_t ratio_denominator = ratio_denom[midi_note_number];

    uint_fast32_t point_in_cycle = ((time * ratio_numerator) / ratio_denominator) & 0b11111111;

    uint32_t result = (current_wave1[point_in_cycle] * wave_blend_syn) + (current_wave2[point_in_cycle] * wave_blend_pair_syn);
    result = (result * multiply_val) >> (MULTIPLIER_WIDTH + BLEND_VAL_MAX_BITS);
    return result;
}

static inline uint_fast16_t audio_sample_get(uint32_t time) {
    uint32_t data = 0;

    // Combine all enabled voices
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
    
    // TODO: remove the right shift after reducing wavetables to 8 bits
    return (uint_fast16_t) (data >> 8);
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

        for (uint_fast16_t i = 0; i < AUDIO_BUF_SIZE * NUM_DAC_CHANNELS; i += NUM_DAC_CHANNELS) {
            uint_fast16_t data = audio_sample_get(time);
            
            data_array[i] = (uint8_t) data;
        
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
        .chan_mask = DAC_CHANNEL_MASK_CH1,
        .desc_num = 16,
        .buf_size = AUDIO_BUF_SIZE * NUM_DAC_CHANNELS,
        .freq_hz = OUTPUT_SAMPLE_RATE,
        .offset = 0,
        .clk_src = DAC_DIGI_CLK_SRC_APLL,
        .chan_mode = DAC_CHANNEL_MODE_SIMUL,
    };
    ESP_ERROR_CHECK(dac_continuous_new_channels(&dac_config, &dac_handle));


    ESP_ERROR_CHECK(dac_continuous_enable(dac_handle));
}
