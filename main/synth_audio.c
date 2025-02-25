#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "driver/dac_oneshot.h"
#include "driver/gptimer.h"
#include "waveform_tables.h"
#include "uart_handler.h"
#include "synth_audio.h"
#include "note_handler.h"
#include "global_header.h"


#define GET_NOTE(i) \
if (note_properties[i].is_sounding == true) {\
    data += wave(note_properties[i].note_num, note_properties[i].multiplier, time);\
    times_added++;\
}


dac_oneshot_handle_t lower_dac_handle;
dac_oneshot_handle_t upper_dac_handle;
gptimer_handle_t wave_gen_timer;

extern const uint32_t sin_array[];
extern const uint32_t ratio_num[];
extern const uint32_t ratio_denom[];
extern const uint32_t sawtooth_array[];


static inline uint_fast32_t wave(uint_fast8_t midi_note_number, uint_fast16_t multiply_val, uint_fast32_t time) {
    uint_fast32_t ratio_numerator = ratio_num[midi_note_number];
    uint_fast32_t ratio_denominator = ratio_denom[midi_note_number];

    uint_fast32_t point_in_cycle = ((time * ratio_numerator) / ratio_denominator) & 0b11111111;

    uint_fast32_t result = sin_array[point_in_cycle];
    result = (result * multiply_val) >> MULTIPLIER_WIDTH;

    return result;
}

void task_audio_generate() {
    while (1) {
        uint64_t time;
        gptimer_get_raw_count(wave_gen_timer, &time);

        uint32_t data = 0;
        uint_fast8_t times_added = 0;

        #if NUM_VOICES >= 1
            GET_NOTE(0)
        #endif
        #if NUM_VOICES >= 2
            GET_NOTE(1)
        #endif
        #if NUM_VOICES >= 3
            GET_NOTE(2)
        #endif
        #if NUM_VOICES >= 4
            GET_NOTE(3)
        #endif
        #if NUM_VOICES >= 5
            GET_NOTE(4)
        #endif
        #if NUM_VOICES >= 6
            GET_NOTE(5)
        #endif
        #if NUM_VOICES >= 7
            GET_NOTE(6)
        #endif
        #if NUM_VOICES >= 8
            GET_NOTE(7)
        #endif
        
        if (times_added != 0) data /= times_added;

        ESP_ERROR_CHECK(dac_oneshot_output_voltage(lower_dac_handle, (uint8_t) data));
        ESP_ERROR_CHECK(dac_oneshot_output_voltage(upper_dac_handle, (uint8_t) (data >> 8)));
    }
}

void wave_timer_init() {
    // wave_gen_timer init
    gptimer_config_t wave_gen_timer_config = {
        .clk_src = GPTIMER_CLK_SRC_APB,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = TIMER_FREQ,
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&wave_gen_timer_config, &wave_gen_timer));

    ESP_ERROR_CHECK(gptimer_enable(wave_gen_timer));
    ESP_ERROR_CHECK(gptimer_start(wave_gen_timer));
}

void dac_init() {
    // Configure DAC for lower 8 bits of output signal
    dac_oneshot_config_t lower_dac_config = {
        .chan_id = DAC_CHAN_0,
    };
    ESP_ERROR_CHECK(dac_oneshot_new_channel(&lower_dac_config, &lower_dac_handle));
    
    // Configure DAC for upper 8 bits of output signal
    dac_oneshot_config_t upper_dac_config = {
        .chan_id = DAC_CHAN_1,
    };
    ESP_ERROR_CHECK(dac_oneshot_new_channel(&upper_dac_config, &upper_dac_handle));
}
