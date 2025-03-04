#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_err.h"
#include "freertos/semphr.h"
#include "basic_io.h"
#include "uart_handler.h"
#include "synth_audio.h"
#include "note_handler.h"
#include "data_y_splitter.h"
#include "global_header.h"
#include "sequencer.h"



const static char *TAG = "BASIC IO";


// Always accessible values
menu_state menu_select = madsr;
int_fast16_t low_pass_val = DEFAULT_LOW_PASS_VAL;

// ADSR menu values
int_fast16_t attack_val = DEFAULT_ENVELOPE_VALS;
int_fast16_t decay_val = DEFAULT_ENVELOPE_VALS;
int_fast16_t sustain_val = DEFAULT_ENVELOPE_VALS;
int_fast16_t release_val = DEFAULT_ENVELOPE_VALS;

// Wave menu values
int_fast16_t wave_select_val = 1;
int_fast16_t high_pass_val = DEFAULT_HIGH_PASS_VAL;

// Sequencer setup values
int_fast16_t squ_enable_val = 0;
int_fast16_t squ_length_val = 64;
int_fast16_t squ_tempo_val = 120;
int_fast16_t squ_duration_val = 50;

// Current channel state
static adc_channel_t current_channel = MENU_POT_1;
// ADC handle
static adc_oneshot_unit_handle_t menu_adc_handle;
    
// Calibration handles
static adc_cali_handle_t menu_pot_1_cali_handle = NULL;
static adc_cali_handle_t menu_pot_2_cali_handle = NULL;
static adc_cali_handle_t menu_pot_3_cali_handle = NULL;
static adc_cali_handle_t menu_pot_4_cali_handle = NULL;
static adc_cali_handle_t menu_low_pass_cali_handle = NULL;
static adc_cali_handle_t menu_select_cali_handle = NULL;

// Calibration ready variables
static bool do_pot_1_calibration;
static bool do_pot_2_calibration;
static bool do_pot_3_calibration;
static bool do_pot_4_calibration;
static bool do_low_pass_calibration;
static bool do_select_calibration;

extern note_data note_properties[];

enum rotary_state_t {
    rwait,
    rcounter1,
    rcounter2,
    rcounter3,
    rforward1,
    rforward2,
    rforward3,
} typedef rotary_state;

// Last states of every rotary encoder
static rotary_state pot_1_last_rotary = rwait;
static rotary_state pot_2_last_rotary = rwait;
static rotary_state pot_3_last_rotary = rwait;
static rotary_state pot_4_last_rotary = rwait;
static rotary_state low_pass_last_rotary = rwait;
static rotary_state select_last_rotary = rwait;

static int32_t rotary_encoder_interpret(uint_fast32_t mv_voltage, rotary_state* state);
static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle);

void apply_low_pass(uint8_t val_to_apply);
void apply_high_pass(uint8_t val_to_apply);


// Save diagnostic stack
#pragma GCC diagnostic push
// Ignore maybe-unintialized variables, since
// GCC complains about pot_1_delta, pot_2_delta, etc...
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"


// Saturation addition with user specified lower and upper bounds (both inclusive)
static int saturation_add(int val_1, int val_2, int lower_bound, int upper_bound) {
    int result = val_1 + val_2;
    if (result < lower_bound) result = lower_bound;
    else if (result > upper_bound) result = upper_bound;
    return result;
}

static void apply_deltas(int* pot_1_delta, int* pot_2_delta, int* pot_3_delta, int* pot_4_delta, int* pot_low_pass_delta, int* pot_select_delta) {

    if (*pot_low_pass_delta) {
        low_pass_val = saturation_add(low_pass_val, *pot_low_pass_delta, 0, LEDC_DUTY_MAX_VAL);
        apply_low_pass((uint8_t) low_pass_val);
    }
    menu_select = saturation_add(menu_select, *pot_select_delta, 0, MAX_MENU_STATE_VAL);
    switch (menu_select) {
        case madsr:
            attack_val = saturation_add(attack_val, *pot_1_delta, MIN_ENVELOPE_VAL, MAX_ENVELOPE_VAL);
            decay_val = saturation_add(decay_val, *pot_2_delta, MIN_ENVELOPE_VAL, MAX_ENVELOPE_VAL);
            sustain_val = saturation_add(sustain_val, *pot_3_delta, MIN_ENVELOPE_VAL, MAX_ENVELOPE_VAL);
            release_val = saturation_add(release_val, *pot_4_delta, MIN_ENVELOPE_VAL, MAX_ENVELOPE_VAL);

            break;
        case mwave:
            wave_select_val = saturation_add(wave_select_val, *pot_1_delta, 0, 3);
            if (*pot_2_delta) {
                high_pass_val = saturation_add(high_pass_val, *pot_2_delta, 0, LEDC_DUTY_MAX_VAL);
                apply_high_pass((uint8_t) high_pass_val);
            }

            break;
        case msequencer_setup:
            if (*pot_1_delta != 0) {
                squ_enable_val = !squ_enable_val;
                if (squ_enable_val) {
                    resume_squ_timer(squ_tempo_val);
                } else {
                    pause_squ_timer();
                }
            }
            squ_length_val = saturation_add(squ_length_val, *pot_2_delta, 0, SEQ_LEN);
            if (*pot_3_delta) {
                squ_tempo_val = saturation_add(squ_tempo_val, *pot_3_delta, 1, 255);
                update_squ_timer(squ_tempo_val);
            }
            squ_duration_val = saturation_add(squ_duration_val, *pot_4_delta, 1, 255);

            break;
        default:
            menu_select = madsr;
            break;
    }

    if (*pot_1_delta | *pot_2_delta | *pot_3_delta | *pot_4_delta | *pot_low_pass_delta | *pot_select_delta) {
        //printf("\033[2JAttack: %03d  Decay: %03d  Sustain: %03d  Release: %03d\nLow pass: %03d  Current menu: %03d\nWave type: %03d\nSequencer enable: %03d  Sequencer clear: %03d\nNote 1: %03d  Note 2: %03d  Note 3: %03d  Note 4: %03d\nNote 5: %03d  Note 6: %03d  Note 7: %03d  Note 8: %03d\n", attack_val, decay_val, sustain_val, release_val, low_pass_val, menu_select, wave_select_val, sequencer_enable_val, sequencer_clear_val, squ_note_1_val, squ_note_2_val, squ_note_3_val, squ_note_4_val, squ_note_5_val, squ_note_6_val, squ_note_7_val, squ_note_8_val);
        // Allow task_data_split() to execute
        xSemaphoreGive(ySplitterSemaphore);
    }
    
    *pot_1_delta =
        *pot_2_delta =
        *pot_3_delta =
        *pot_4_delta =
        *pot_low_pass_delta =
        *pot_select_delta =
        0;
}

void apply_low_pass(uint8_t val_to_apply) {
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LOW_PASS_LEDC_CHANNEL, val_to_apply));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LOW_PASS_LEDC_CHANNEL));
}

void apply_high_pass(uint8_t val_to_apply) {
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, HIGH_PASS_LEDC_CHANNEL, val_to_apply));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, HIGH_PASS_LEDC_CHANNEL));
}

void task_adc() {

    int pot_1_delta, pot_2_delta, pot_3_delta, pot_4_delta, pot_low_pass_delta, pot_select_delta = 0;

    while (1) {
        
        int adc_result = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(menu_adc_handle, current_channel, &adc_result));

        switch (current_channel) {
            case MENU_POT_1:
                if (do_pot_1_calibration)
                    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(menu_pot_1_cali_handle, adc_result, &adc_result));
                pot_1_delta = rotary_encoder_interpret(adc_result, &pot_1_last_rotary);
                current_channel = MENU_POT_2;

                break;
            case MENU_POT_2:
                if (do_pot_2_calibration)
                    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(menu_pot_2_cali_handle, adc_result, &adc_result));
                pot_2_delta = rotary_encoder_interpret(adc_result, &pot_2_last_rotary);
                current_channel = MENU_POT_3;

                break;
            case MENU_POT_3:
                if (do_pot_3_calibration)
                    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(menu_pot_3_cali_handle, adc_result, &adc_result));
                pot_3_delta = rotary_encoder_interpret(adc_result, &pot_3_last_rotary);
                current_channel = MENU_POT_4;

                break;
            case MENU_POT_4:
                if (do_pot_4_calibration)
                    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(menu_pot_4_cali_handle, adc_result, &adc_result));
                pot_4_delta = rotary_encoder_interpret(adc_result, &pot_4_last_rotary);
                current_channel = LOW_PASS_POT;

                break;
            case LOW_PASS_POT:
                if (do_low_pass_calibration)
                    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(menu_low_pass_cali_handle, adc_result, &adc_result));
                pot_low_pass_delta = rotary_encoder_interpret(adc_result, &low_pass_last_rotary);
                current_channel = SELECT_POT;

                break;
            case SELECT_POT:
                if (do_select_calibration)
                    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(menu_select_cali_handle, adc_result, &adc_result));
                pot_select_delta = rotary_encoder_interpret(adc_result, &select_last_rotary);
                current_channel = MENU_POT_1;
                
                apply_deltas(&pot_1_delta, &pot_2_delta, &pot_3_delta, &pot_4_delta, &pot_low_pass_delta, &pot_select_delta);

                break;
            default:
                current_channel = MENU_POT_1;

                break;
        }


        vTaskDelay(pdMS_TO_TICKS(ANALOG_READ_LOOP_MS));
    }
}

void gpio_interrupt_handler(void *args) {
    int gpio_num = (int) args;
    switch (gpio_num) {
        case MIDI_PANIC_BUTTON:
            for (int i = 0; i < NUM_VOICES; i++) note_properties[i].is_sounding = false;

            // Test for PWM signals
            ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, STATUS_LEDC_CHANNEL, 0b00111111));
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, STATUS_LEDC_CHANNEL));
            ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, HIGH_PASS_LEDC_CHANNEL, 0b00111111));
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, HIGH_PASS_LEDC_CHANNEL));
            ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LOW_PASS_LEDC_CHANNEL, 0b00111111));
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LOW_PASS_LEDC_CHANNEL));

            // Test to ensure midi panic button and clipping outputs are functional
            ESP_ERROR_CHECK(gpio_set_level(HARD_CLIPPING_EN, 1));
            ESP_ERROR_CHECK(gpio_set_level(SOFT_CLIPPING_EN, 0));
            break;
        case EXTRA_BUTTON:
            // Test for PWM signals
            ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, STATUS_LEDC_CHANNEL, 0b00000111));
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, STATUS_LEDC_CHANNEL));
            ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, HIGH_PASS_LEDC_CHANNEL, 0b00000111));
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, HIGH_PASS_LEDC_CHANNEL));
            ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LOW_PASS_LEDC_CHANNEL, 0b00000111));
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LOW_PASS_LEDC_CHANNEL));

            // Test to ensure extra button and clipping outputs are functional
            ESP_ERROR_CHECK(gpio_set_level(SOFT_CLIPPING_EN, 1));
            ESP_ERROR_CHECK(gpio_set_level(HARD_CLIPPING_EN, 0));
            break;
    }

}


// Interpret rotary encoder value changes with tracked last state of target rotary encoder
static int32_t rotary_encoder_interpret(uint_fast32_t mv_voltage, rotary_state* state) {
    switch (*state) {
        case rwait:
            if (mv_voltage > ROTARY_LOW_MAX) {
                if (mv_voltage < ROTARY_MIDLOW_TARGET + ROTARY_DELTA) {
                    *state = rforward1;
                }
                else if (mv_voltage < ROTARY_MIDHIGH_TARGET + ROTARY_DELTA) {
                    *state = rcounter1;
                }
            }
            break;
        case rcounter1:
            if (mv_voltage < ROTARY_LOW_MAX) {
                *state = rcounter2;
            }
            break;
        case rcounter2:
            if (mv_voltage > ROTARY_MIDLOW_TARGET - ROTARY_DELTA) {
                *state = rcounter3;
                return -1;
            }
            break;
        case rcounter3:
            if (mv_voltage > ROTARY_HIGH_MIN) {
                *state = rwait;
            }
            break;
        case rforward1:
            if (mv_voltage < ROTARY_LOW_MAX) {
                *state = rforward2;
            }
            break;
        case rforward2:
            if (mv_voltage > ROTARY_MIDLOW_TARGET - ROTARY_DELTA) {
                *state = rforward3;
                return 1;
            }
            break;
        case rforward3:
            if (mv_voltage > ROTARY_HIGH_MIN) {
                *state = rwait;
            }
            break;
        default:
            *state = rwait;
    }
    return 0;
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
    ESP_ERROR_CHECK(adc_oneshot_config_channel(menu_adc_handle, LOW_PASS_POT, &menu_channel_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(menu_adc_handle, SELECT_POT, &menu_channel_config));

    do_pot_1_calibration = adc_calibration_init(MENU_UNIT, MENU_POT_1, MENU_ATTEN, &menu_pot_1_cali_handle);
    do_pot_2_calibration = adc_calibration_init(MENU_UNIT, MENU_POT_2, MENU_ATTEN, &menu_pot_2_cali_handle);
    do_pot_3_calibration = adc_calibration_init(MENU_UNIT, MENU_POT_3, MENU_ATTEN, &menu_pot_3_cali_handle);
    do_pot_4_calibration = adc_calibration_init(MENU_UNIT, MENU_POT_4, MENU_ATTEN, &menu_pot_4_cali_handle);
    do_low_pass_calibration = adc_calibration_init(MENU_UNIT, LOW_PASS_POT, MENU_ATTEN, &menu_low_pass_cali_handle);
    do_select_calibration = adc_calibration_init(MENU_UNIT, SELECT_POT, MENU_ATTEN, &menu_select_cali_handle);
}

// Restore diagnostic stack
// NOTE: always keep this directly after task_adc()
#pragma GCC diagnostic pop

void ledc_init() {
    ledc_timer_config_t ledc_timer_conf = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = LEDC_DUTY_RES,
        .timer_num = LEDC_TIMER,
        .freq_hz = 312500, // 40000000 / 2^7
        .clk_cfg = LEDC_AUTO_CLK,
    };

    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer_conf));

    // Configure status led (gpio 2) which is built into esp32 board
    ledc_channel_config_t status_ledc_channel_conf = {
        .speed_mode = LEDC_MODE,
        .channel = STATUS_LEDC_CHANNEL,
        .timer_sel = LEDC_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = STATUS_LEDC,
        .duty = 0,
        .hpoint = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&status_ledc_channel_conf));

    // Configure low-pass pwm control
    ledc_channel_config_t low_pass_ledc_channel_conf = {
        .speed_mode = LEDC_MODE,
        .channel = LOW_PASS_LEDC_CHANNEL,
        .timer_sel = LEDC_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = LOW_PASS_LEDC,
        .duty = 0,
        .hpoint = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&low_pass_ledc_channel_conf));

    // Configure high-pass pwm control
    ledc_channel_config_t high_pass_ledc_channel_conf = {
        .speed_mode = LEDC_MODE,
        .channel = HIGH_PASS_LEDC_CHANNEL,
        .timer_sel = LEDC_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = HIGH_PASS_LEDC,
        .duty = 0,
        .hpoint = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&high_pass_ledc_channel_conf));
}



void gpio_init() {
    gpio_config_t io_input_conf = {
        .intr_type = GPIO_INTR_POSEDGE,
        .pin_bit_mask = GPIO_INPUT_PIN_SEL,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 1,
        .pull_down_en = 0,
    };
    gpio_config(&io_input_conf);

    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(MIDI_PANIC_BUTTON, gpio_interrupt_handler, (void *) MIDI_PANIC_BUTTON);
    gpio_isr_handler_add(EXTRA_BUTTON, gpio_interrupt_handler, (void *) EXTRA_BUTTON);


    gpio_config_t io_output_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .pin_bit_mask = GPIO_OUTPUT_PIN_SEL,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
    };
    gpio_config(&io_output_conf);
}

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
