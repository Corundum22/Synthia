#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/dac_oneshot.h"
#include "driver/gptimer.h"

#define TIMER_FREQ 80000000 // 80 MHz

static dac_oneshot_handle_t dac_handle;
static gptimer_handle_t wave_gen_timer;
static TaskHandle_t audio_task_handle;

uint8_t sin_array[] = { 127, 130, 133, 136, 139, 143, 146, 149, 
    152, 155, 158, 161, 164, 167, 170, 173, 176, 179, 182, 184, 
    187, 190, 193, 195, 198, 200, 203, 205, 208, 210, 213, 215, 
    217, 219, 221, 224, 226, 228, 229, 231, 233, 235, 236, 238, 
    239, 241, 242, 244, 245, 246, 247, 248, 249, 250, 251, 251, 
    252, 253, 253, 254, 254, 254, 254, 254, 255, 254, 254, 254, 
    254, 254, 253, 253, 252, 251, 251, 250, 249, 248, 247, 246, 
    245, 244, 242, 241, 239, 238, 236, 235, 233, 231, 229, 228, 
    226, 224, 221, 219, 217, 215, 213, 210, 208, 205, 203, 200, 
    198, 195, 193, 190, 187, 184, 182, 179, 176, 173, 170, 167, 
    164, 161, 158, 155, 152, 149, 146, 143, 139, 136, 133, 130, 
    127, 124, 121, 118, 115, 111, 108, 105, 102, 99, 96, 93, 
    90, 87, 84, 81, 78, 75, 72, 70, 67, 64, 61, 59, 56, 54, 
    51, 49, 46, 44, 41, 39, 37, 35, 33, 30, 28, 26, 25, 23, 
    21, 19, 18, 16, 15, 13, 12, 10, 9, 8, 7, 6, 5, 4, 3, 3, 
    2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 3, 
    4, 5, 6, 7, 8, 9, 10, 12, 13, 15, 16, 18, 19, 21, 23, 25, 
    26, 28, 30, 33, 35, 37, 39, 41, 44, 46, 49, 51, 54, 56, 
    59, 61, 64, 67, 70, 72, 75, 78, 81, 84, 87, 90, 93, 96, 
    99, 102, 105, 108, 111, 115, 118, 121, 124 };

static uint32_t sawtooth_array[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 
    12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 
    26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 
    40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 
    54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 
    68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 
    82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 
    96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 
    108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 
    120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 
    132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 
    144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 
    156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 
    168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 
    180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 
    204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 
    216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 
    228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 
    252, 253, 254, 255 };

uint32_t midi_note_to_strange_period[] = { 38203, 36085, 
    34041, 32150, 30340, 28643, 27033, 25510, 24076, 22727, 21448,
    20253, 19113, 18043, 17030, 16067, 15170, 14315, 13516, 12755,
    12038, 11364, 10724, 10123, 9557, 9019, 8513, 8035, 7585,
    7159, 6757, 6378, 6020, 5682, 5363, 5062, 4778, 4509, 4256,
    4018, 3792, 3579, 3378, 3189, 3010, 2841, 2681, 2531, 2389,
    2255, 2128, 2009, 1896, 1790, 1689, 1594, 1505, 1420, 1341,
    1265, 1194, 1127, 1064, 1004, 948, 895, 845, 797, 752, 710,
    670, 633, 597, 564, 532, 502, 474, 447, 422, 399, 376, 355,
    338, 316, 299, 282, 266, 251, 237, 224, 211, 199, 188, 178, 168,
    158, 149, 141, 133, 126, 119, 112, 106, 100, 94, 89, 84, 79,
    75, 70, 67, 63, 59, 56, 53, 50, 47, 44, 42, 40, 37, 35, 33,
    31, 30, 28, 26, 25 };

static inline uint32_t wave(uint16_t strange_period, uint64_t time) {
    //uint32_t point_in_cycle = (time >> 7) & 0b11111111; // gives 30.52 Hz
    uint32_t point_in_cycle = (time * strange_period) & 0b11111111;
    /*
     * MAX_FREQ: the maximum frequency to completely cycle through the array
     * ARRAY_LEN: the length of the array
     * ARRAY_LEN_BITS: the number of bits needed to access the entire array
     * ARRAY_LEN = 2^ARRAY_LEN_BITS
     * MAX_FREQ = TIMER_FREQ / 2^ARRAY_LEN_BITS = TIMER_FREQ / ARRAY_LEN
     * achieved_freq = MAX_FREQ / strange_period
     *
     * tones_from_max: the number of tones away (under) MAX_FREQ
     * TONES_PER_OCTAVE: the number of tones per octave (e.g. TONES_PER_OCTAVE = 12 for 12EDO)
     * achieved_freq = MAX_FREQ * 2^(tones_from_max / TONES_PER_OCTAVE)
     * strange_period = 2^(TONES_PER_OCTAVE / tones_from_max)
     * */

    return sin_array[point_in_cycle];
}

void task_audio_generate() {
    while (1) {
        uint64_t time;
        gptimer_get_raw_count(wave_gen_timer, &time);

        uint8_t data = (uint8_t) wave(midi_note_to_strange_period[50], time);

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
