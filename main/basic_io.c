#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_err.h"
#include "basic_io.h"
#include "uart_handler.h"


uint_fast8_t menu_select = 0;

uint_fast16_t sin_volume = 0;
uint_fast16_t sin_1 = 0;
uint_fast16_t sin_2 = 0;
uint_fast16_t sin_3 = 0;

adc_channel_t current_channel = MENU_POT_1;
adc_oneshot_unit_handle_t menu_adc_handle;

extern uint_fast8_t on_notes[];



void task_adc() {

    uint32_t pot_1 = 0;
    uint32_t pot_2 = 0;
    uint32_t pot_3 = 0;
    uint32_t pot_4 = 0;
    uint32_t pot_vol = 0;
    uint32_t pot_low_pass = 0;

    while (1) {
        int adc_result = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(menu_adc_handle, current_channel, &adc_result));

        switch (current_channel) {
            case MENU_POT_1:
                pot_1 = adc_result;
                current_channel = MENU_POT_2;
                break;
            case MENU_POT_2:
                pot_2 = adc_result;
                current_channel = MENU_POT_3;
                break;
            case MENU_POT_3:
                pot_3 = adc_result;
                current_channel = MENU_POT_4;
                break;
            case MENU_POT_4:
                pot_4 = adc_result;
                current_channel = VOL_POT;
                break;
            case VOL_POT:
                pot_vol = adc_result;
                current_channel = LOW_PASS_POT;
                break;
            case LOW_PASS_POT:
                pot_low_pass = adc_result;
                printf("%06u %06u %06u %06u\r", sin_volume, sin_1, sin_2, sin_3); // TODO: remove line later
                current_channel = MENU_POT_1;
                break;
            default:
                current_channel = MENU_POT_1;
                break;
        }

        switch (menu_select) {
            case 0:
                sin_volume = pot_1;
                sin_1 = pot_2;
                sin_2 = pot_3;
                sin_3 = pot_4;
                break;
            default:
                break;
        }

        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void adc_init() {
    adc_oneshot_unit_init_cfg_t menu_adc_config = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&menu_adc_config, &menu_adc_handle));

    adc_oneshot_chan_cfg_t menu_channel_config = {
        .atten = MENU_ATTEN,
        .bitwidth = MENU_BITS,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(menu_adc_handle, MENU_POT_1, &menu_channel_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(menu_adc_handle, MENU_POT_2, &menu_channel_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(menu_adc_handle, MENU_POT_3, &menu_channel_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(menu_adc_handle, MENU_POT_4, &menu_channel_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(menu_adc_handle, VOL_POT, &menu_channel_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(menu_adc_handle, LOW_PASS_POT, &menu_channel_config));
}


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
        case MIDI_PANIC_BUTTON:
            for (int i = 0; i < NUM_VOICES; i++) on_notes[i] = 0;
            ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0b00111111));
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
            break;
    }

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
    gpio_isr_handler_add(MIDI_PANIC_BUTTON, gpio_interrupt_handler, (void *) MIDI_PANIC_BUTTON);
}
