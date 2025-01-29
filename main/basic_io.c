#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "basic_io.h"
#include "synth_audio.h"

extern uint16_t current_note;

void ledc_init() {
    ledc_timer_config_t ledc_timer_conf = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = LEDC_DUTY_RES,
        .timer_num = LEDC_TIMER,
        .freq_hz = 312500, // 40000000 / 2^7
        .clk_cfg = LEDC_AUTO_CLK,
    };

    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer_conf));

    ledc_channel_config_t ledc_channel_conf = {
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL,
        .timer_sel = LEDC_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = 2,
        .duty = 0,
        .hpoint = 0,
    };

    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_conf));
}


void gpio_interrupt_handler(void *args) {
    int gpio_num = (int) args;
    switch (gpio_num) {
        case 12:
            current_note++;
            break;
        case 14:
            current_note--;
            break;
    }

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, (current_note * 10) & 0b01111111));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}

void gpio_init() {
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
