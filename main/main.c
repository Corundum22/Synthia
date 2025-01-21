#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/dac_oneshot.h"
#include "driver/gptimer.h"
#include "driver/gpio.h"

#define TIMER_FREQ 40000000 // 40 MHz
#define FREQ_UP_BUTTON 12
#define FREQ_DOWN_BUTTON 14
#define GPIO_INPUT_PIN_SEL ((1ULL<<FREQ_UP_BUTTON) | (1ULL<<FREQ_DOWN_BUTTON))
#define ESP_INTR_FLAG_DEFAULT 0

static dac_oneshot_handle_t dac_handle;
static gptimer_handle_t wave_gen_timer;
static TaskHandle_t audio_task_handle;
static uint16_t current_note = 69;

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

uint32_t ratio_num_denom[] = { 745340, 1641517, 1413176, 369624, 
    788769, 1360142, 1337858, 51021, 1191896, 125000, 482663, 
    536564, 372670, 333715, 706588, 184812, 477812, 680071, 
    668929, 51021, 595948, 62500, 482663, 268282, 186335, 333715, 
    353294, 92406, 238906, 289925, 337843, 51021, 297974, 31250, 
    34859, 134141, 186335, 160093, 176647, 46203, 119453, 100221, 
    23649, 51021, 148987, 15625, 34859, 99973, 107501, 86811, 
    69169, 46203, 72051, 47426, 23649, 51021, 27841, 15625, 
    34859, 17084, 47181, 36641, 38309, 46203, 23701, 23713, 
    23649, 39063, 27841, 15625, 31172, 8542, 13139, 25085, 7715, 
    24357, 23701, 14541, 16681, 18535, 16366, 15625, 15586, 
    4271, 13139, 5778, 7715, 3641, 11732, 4586, 3484, 8271, 
    8183, 8434, 7793, 4271, 5599, 2889, 3791, 3641, 5866, 2293, 
    1742, 1993, 1646, 4217, 3687, 3836, 1829, 2889, 1962, 2103, 
    2933, 2293, 871, 1993, 823, 799, 2053, 1918, 1829, 1744, 
    981, 769, 1348, 769, 871, 710, 39, 91, 83, 23, 52, 95, 99, 
    4, 99, 11, 45, 53, 39, 37, 83, 23, 63, 95, 99, 8, 99, 11, 
    90, 53, 39, 74, 83, 23, 63, 81, 100, 16, 99, 11, 13, 53, 
    78, 71, 83, 23, 63, 56, 14, 32, 99, 11, 26, 79, 90, 77, 
    65, 46, 76, 53, 28, 64, 37, 22, 52, 27, 79, 65, 72, 92, 
    50, 53, 56, 98, 74, 44, 93, 27, 44, 89, 29, 97, 100, 65, 
    79, 93, 87, 88, 93, 27, 88, 41, 58, 29, 99, 41, 33, 83, 
    87, 95, 93, 54, 75, 41, 57, 58, 99, 41, 33, 40, 35, 95, 
    88, 97, 49, 82, 59, 67, 99, 82, 33, 80, 35, 36, 98, 97, 
    98, 99, 59, 49, 91, 55, 66, 57 };

/*static inline uint32_t wave(uint32_t ratio_numerator, uint32_t ratio_denominator, uint32_t time) {
    uint32_t point_in_cycle = ((time * ratio_numerator) / ratio_denominator) & 0b11111111;

    return sin_array[point_in_cycle];
}*/
static inline uint32_t wave(uint32_t midi_note_number, uint32_t time) {
    uint32_t ratio_numerator = ratio_num_denom[midi_note_number + 128];
    uint32_t ratio_denominator = ratio_num_denom[midi_note_number];
    uint32_t point_in_cycle = ((time * ratio_numerator) / ratio_denominator) & 0b11111111;

    return sin_array[point_in_cycle];
}

void task_audio_generate() {
    while (1) {
        uint64_t time;
        gptimer_get_raw_count(wave_gen_timer, &time);

        //uint8_t data = (uint8_t) wave(100, (uint32_t) time); // gives 1.56 kHz
        //uint8_t data = (uint8_t) wave(midi_note_to_strange_period[current_note] >> 1, (uint32_t) time);

        uint8_t data = (uint8_t) wave(current_note & 0b01111111, (uint32_t) time);
        data = (uint8_t) wave(current_note & 0b01111111, (uint32_t) time);
        data = (uint8_t) wave(current_note & 0b01111111, (uint32_t) time);
        data = (uint8_t) wave(current_note & 0b01111111, (uint32_t) time);
        data = (uint8_t) wave(current_note & 0b01111111, (uint32_t) time);
        data = (uint8_t) wave(current_note & 0b01111111, (uint32_t) time);
        data = (uint8_t) wave(current_note & 0b01111111, (uint32_t) time);
        data = (uint8_t) wave(current_note & 0b01111111, (uint32_t) time);

        ESP_ERROR_CHECK(dac_oneshot_output_voltage(dac_handle, data));
    }
}

static void IRAM_ATTR gpio_interrupt_handler(void *args) {
    int gpio_num = (int) args;
    switch (gpio_num) {
        case 12:
            current_note++;
            break;
        case 14:
            current_note--;
            break;
    }
}

static void timer_init() {
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

static void dac_init() {
    dac_oneshot_config_t dac_config = {
        .chan_id = DAC_CHAN_0,
    };
    ESP_ERROR_CHECK(dac_oneshot_new_channel(&dac_config, &dac_handle));
}

static void gpio_init() {
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_POSEDGE,
        .pin_bit_mask = GPIO_INPUT_PIN_SEL,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 1,
    };
    gpio_config(&io_conf);

    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(FREQ_UP_BUTTON, gpio_interrupt_handler, (void *) FREQ_UP_BUTTON);
    gpio_isr_handler_add(FREQ_DOWN_BUTTON, gpio_interrupt_handler, (void *) FREQ_DOWN_BUTTON);
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

    gpio_init();
    printf("GPIO init finished\n");

    task_create();
}
