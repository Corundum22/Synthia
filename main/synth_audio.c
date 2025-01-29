#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "driver/dac_oneshot.h"
#include "driver/gptimer.h"
#include "waveform_tables.h"

#include "synth_audio.h"


dac_oneshot_handle_t dac_handle;
gptimer_handle_t wave_gen_timer;
uint16_t current_note = 69;

extern const uint32_t sin_array[];
extern const uint32_t ratio_num_denom[];
extern const uint32_t sawtooth_array[];


uint32_t wave(uint32_t midi_note_number, uint32_t time) {
    uint32_t ratio_numerator = ratio_num_denom[midi_note_number + 128];
    uint32_t ratio_denominator = ratio_num_denom[midi_note_number];

    uint32_t point_in_cycle = ((time * ratio_numerator) / ratio_denominator) & 0b11111111;

    return sin_array[point_in_cycle];
}

void task_audio_generate() {
    while (1) {
        uint64_t time;
        gptimer_get_raw_count(wave_gen_timer, &time);

        uint8_t data = (uint8_t) wave(current_note & 0b01111111, (uint32_t) time);

        ESP_ERROR_CHECK(dac_oneshot_output_voltage(dac_handle, data));
    }
}

void timer_init() {
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
