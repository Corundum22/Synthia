#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_err.h"
#include "basic_io.h"
#include "uart_handler.h"
#include "synth_audio.h"
#include "note_handler.h"


const static char *TAG = "BASIC IO";

uint_fast8_t menu_select = 0;

uint_fast16_t wave_1_vol = 0;
uint_fast16_t wave_1_type = 0;
uint_fast16_t wave_1_harm = 0;
uint_fast16_t wave_1_detune = 0;

adc_channel_t current_channel = MENU_POT_1;
adc_oneshot_unit_handle_t menu_adc_handle;
    
static adc_cali_handle_t menu_pot_1_cali_handle = NULL;
static adc_cali_handle_t menu_pot_2_cali_handle = NULL;
static adc_cali_handle_t menu_pot_3_cali_handle = NULL;
static adc_cali_handle_t menu_pot_4_cali_handle = NULL;
static adc_cali_handle_t menu_vol_pot_cali_handle = NULL;
static adc_cali_handle_t menu_low_pass_cali_handle = NULL;

bool do_pot_1_calibration;
bool do_pot_2_calibration;
bool do_pot_3_calibration;
bool do_pot_4_calibration;
bool do_vol_pot_calibration;
bool do_low_pass_calibration;

extern note_data note_properties[];


static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle) {
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .chan = channel,
            .atten = atten,
            .bitwidth = MENU_BITS,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = MENU_BITS,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

    *out_handle = handle;
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Calibration Success");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else {
        ESP_LOGE(TAG, "Invalid arg or no memory");
    }

    return calibrated;
}


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
                if (do_pot_1_calibration)
                    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(menu_pot_1_cali_handle, adc_result, &adc_result));
                pot_1 = adc_result;
                current_channel = MENU_POT_2;
                break;
            case MENU_POT_2:
                if (do_pot_2_calibration)
                    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(menu_pot_2_cali_handle, adc_result, &adc_result));
                pot_2 = adc_result;
                current_channel = MENU_POT_3;
                break;
            case MENU_POT_3:
                if (do_pot_3_calibration)
                    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(menu_pot_3_cali_handle, adc_result, &adc_result));
                pot_3 = adc_result;
                current_channel = MENU_POT_4;
                break;
            case MENU_POT_4:
                if (do_pot_4_calibration)
                    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(menu_pot_4_cali_handle, adc_result, &adc_result));
                pot_4 = adc_result;
                current_channel = VOL_POT;
                break;
            case VOL_POT:
                if (do_vol_pot_calibration)
                    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(menu_vol_pot_cali_handle, adc_result, &adc_result));
                pot_vol = adc_result;
                current_channel = LOW_PASS_POT;
                break;
            case LOW_PASS_POT:
                if (do_low_pass_calibration)
                    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(menu_low_pass_cali_handle, adc_result, &adc_result));
                pot_low_pass = adc_result;
                printf("%06u %06u %06u %06u\r", wave_1_vol, wave_1_type, wave_1_harm, wave_1_detune); // TODO: remove line later
                current_channel = MENU_POT_1;
                break;
            default:
                current_channel = MENU_POT_1;
                break;
        }

        switch (menu_select) {
            case 0:
                wave_1_vol = pot_1;
                wave_1_type = pot_2;
                wave_1_harm = pot_3;
                wave_1_detune = pot_4;
                break;
            default:
                break;
        }

        vTaskDelay(pdMS_TO_TICKS(50));
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

    do_pot_1_calibration = adc_calibration_init(MENU_UNIT, MENU_POT_1, MENU_ATTEN, &menu_pot_1_cali_handle);
    do_pot_2_calibration = adc_calibration_init(MENU_UNIT, MENU_POT_2, MENU_ATTEN, &menu_pot_2_cali_handle);
    do_pot_3_calibration = adc_calibration_init(MENU_UNIT, MENU_POT_3, MENU_ATTEN, &menu_pot_3_cali_handle);
    do_pot_4_calibration = adc_calibration_init(MENU_UNIT, MENU_POT_4, MENU_ATTEN, &menu_pot_4_cali_handle);
    do_vol_pot_calibration = adc_calibration_init(MENU_UNIT, VOL_POT, MENU_ATTEN, &menu_vol_pot_cali_handle);
    do_low_pass_calibration = adc_calibration_init(MENU_UNIT, LOW_PASS_POT, MENU_ATTEN, &menu_low_pass_cali_handle);
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
            for (int i = 0; i < NUM_VOICES; i++) note_properties[i].is_sounding = false;
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
