#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/dac_oneshot.h"
#include "driver/gptimer.h"

#define TIMER_FREQ 1000000 // 1 MHz = 1 µs

static dac_oneshot_handle_t dac_handle;
static gptimer_handle_t wave_gen_timer;
static TaskHandle_t audio_task_handle;

inline uint8_t sawtooth(uint16_t freq, uint64_t time) {
    uint32_t usperiod = TIMER_FREQ / freq;

    uint32_t point_in_cycle = time % usperiod;

    return (uint8_t) ((point_in_cycle * 255) / usperiod);
}

void task_audio_generate() {
    while (1) {
        uint64_t time;
        gptimer_get_raw_count(wave_gen_timer, &time);

        uint8_t data = sawtooth(10000, time);

        ESP_ERROR_CHECK(dac_oneshot_output_voltage(dac_handle, data));
    }
}

static void timer_init() {
    // wave_gen_timer init
    gptimer_config_t wave_gen_timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = TIMER_FREQ,
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&wave_gen_timer_config, &wave_gen_timer));

    ESP_ERROR_CHECK(gptimer_enable(wave_gen_timer));
    ESP_ERROR_CHECK(gptimer_start(wave_gen_timer));
}

static void dac_init() {
    dac_oneshot_config_t dac_config = {
        .chan_id = DAC_CHAN_0,
    };
    ESP_ERROR_CHECK(dac_oneshot_new_channel(&dac_config, &dac_handle));
}

static void task_create() {
    xTaskCreate(task_audio_generate, "Audio Generation Task", 4096, NULL, 10, &audio_task_handle);
}

void app_main(void) {
    printf("Program started\n");

    timer_init();
    printf("Timer init finished\n");

    dac_init();
    printf("DAC init finished\n");

    task_create();
}
