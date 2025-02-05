#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "driver/dac_oneshot.h"
#include "driver/gptimer.h"
#include "waveform_tables.h"
#include "uart_handler.h"
#include "synth_audio.h"
#include "note_handler.h"




dac_oneshot_handle_t dac_handle;
gptimer_handle_t wave_gen_timer;

extern const uint32_t sin_array[];
extern const uint32_t ratio_num[];
extern const uint32_t ratio_denom[];
extern const uint32_t sawtooth_array[];


static inline uint_fast32_t wave(uint_fast8_t midi_note_number, uint_fast32_t time) {
    uint_fast32_t ratio_numerator = ratio_num[midi_note_number];
    uint_fast32_t ratio_denominator = ratio_denom[midi_note_number];

    uint_fast32_t point_in_cycle = ((time * ratio_numerator) / ratio_denominator) & 0b11111111;

    uint_fast32_t result = sin_array[point_in_cycle];

    return result;
}

void task_audio_generate() {
    while (1) {
        uint64_t time;
        gptimer_get_raw_count(wave_gen_timer, &time);

        uint32_t data = 0;
        uint_fast8_t times_added = 0;
        for (int i = 0; i < NUM_VOICES; i++) {
            if (note_properties[i].is_sounding == true) {
                data += wave(note_properties[i].note_num, time) / note_properties[i].divisor;
                times_added++;
            }
        }
        if (times_added != 0) data /= times_added;

        ESP_ERROR_CHECK(dac_oneshot_output_voltage(dac_handle, (uint8_t) data));
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
    dac_oneshot_config_t dac_config = {
        .chan_id = DAC_CHAN_0,
    };
    ESP_ERROR_CHECK(dac_oneshot_new_channel(&dac_config, &dac_handle));
}
