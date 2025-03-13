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
#include <string.h>


#define NUM_COMB_VOICES (NUM_VOICES + SEQ_VOICES)

#define GET_NOTE(i) \
if (note_properties[i].is_sounding == true) {\
    data += wave(note_properties[i].note_num, note_properties[i].multiplier, time);\
}



dac_continuous_handle_t dac_handle;

uint_fast16_t *current_wave = sin_array;


static inline uint16_t wave(uint_fast8_t midi_note_number, uint_fast16_t multiply_val, uint_fast32_t time) {
    uint_fast32_t ratio_numerator = ratio_num[midi_note_number];
    uint_fast32_t ratio_denominator = ratio_denom[midi_note_number];

    uint_fast32_t point_in_cycle = ((time * ratio_numerator) / ratio_denominator) & 0b11111111;

    uint16_t result = current_wave[point_in_cycle];
    result = (result * multiply_val) >> MULTIPLIER_WIDTH;
    return result;
}

static inline uint16_t audio_sample_get(uint32_t time) {
    uint16_t data = 0;

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
    
    return data;
}

static inline void set_current_wave() {
    switch (wave_select_syn) {
        case ssin:
            current_wave = sin_array;
            return;
        case striangle:
            current_wave = triangle_array;
            return;
        case ssawtooth:
            current_wave = sawtooth_array;
            return;
        case ssquare:
            current_wave = square_array;
            return;
        default:
            return;
    }
}

static uint8_t data_array[AUDIO_BUF_SIZE * NUM_DAC_CHANNELS];
void task_audio_generate() {

    uint32_t time = 0;

    while (1) {

        set_current_wave();
 
        for (uint_fast16_t i = 0; i < AUDIO_BUF_SIZE * NUM_DAC_CHANNELS; i += 2) {
            uint16_t data = audio_sample_get(time);
            
            data_array[i] = (uint8_t) data;
            data_array[i + 1] = (uint8_t) (data >> 8);
        
            time++;
        }


        ESP_ERROR_CHECK(dac_continuous_write(dac_handle, data_array, AUDIO_BUF_SIZE * NUM_DAC_CHANNELS, NULL, -1));
   

    }
}

void dac_init() {
    // Configure DAC for lower 8 bits of output signal
    dac_continuous_config_t dac_config = {
        .chan_mask = DAC_CHANNEL_MASK_ALL,
        .desc_num = 4, // TODO: select a proper value for this
        .buf_size = AUDIO_BUF_SIZE * NUM_DAC_CHANNELS,
        .freq_hz = OUTPUT_SAMPLE_RATE,
        .offset = 0,
        .clk_src = DAC_DIGI_CLK_SRC_APLL,
        .chan_mode = DAC_CHANNEL_MODE_ALTER,
    };
    ESP_ERROR_CHECK(dac_continuous_new_channels(&dac_config, &dac_handle));


    ESP_ERROR_CHECK(dac_continuous_enable(dac_handle));
}
