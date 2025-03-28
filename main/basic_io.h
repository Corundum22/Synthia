#ifndef BASIC_IO_H_
#define BASIC_IO_H_

#include <stdint.h>
#include "global_header.h"


#define MIDI_PANIC_BUTTON       21
#define EXTRA_BUTTON            15
#define GPIO_INPUT_PIN_SEL      ((1ULL<<MIDI_PANIC_BUTTON) | (1ULL<<EXTRA_BUTTON))
#define ESP_INTR_FLAG_DEFAULT   0

#define SOFT_CLIPPING_EN        27
#define HARD_CLIPPING_EN        13
#define GPIO_OUTPUT_PIN_SEL      ((1ULL<<SOFT_CLIPPING_EN) | (1ULL<<HARD_CLIPPING_EN))

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_DUTY_RES           LEDC_TIMER_8_BIT // Set duty resolution to 8 bits
#define LEDC_DUTY_MAX_VAL       255
#define LEDC_FREQUENCY          156250 // Set frequency at 156250 Hz

#define STATUS_LEDC_CHANNEL       LEDC_CHANNEL_0
#define HIGH_PASS_LEDC_CHANNEL    LEDC_CHANNEL_1
#define LOW_PASS_LEDC_CHANNEL     LEDC_CHANNEL_2

#define STATUS_LEDC       2
#define HIGH_PASS_LEDC    17
#define LOW_PASS_LEDC     14

#define MENU_POT_1    ADC_CHANNEL_0 // gpio36
#define MENU_POT_2    ADC_CHANNEL_3 // gpio39
#define MENU_POT_3    ADC_CHANNEL_6 // gpio34
#define MENU_POT_4    ADC_CHANNEL_7 // gpio35
#define LOW_PASS_POT  ADC_CHANNEL_4 // gpio32
#define SELECT_POT    ADC_CHANNEL_5 // gpio33

#define MENU_ATTEN    ADC_ATTEN_DB_12
#define MENU_BITS     ADC_BITWIDTH_11
#define MENU_UNIT     ADC_UNIT_1
#define DEFAULT_VREF  1100

#define CHOSEN_ADC_CALI_SCHEME    ESP_ADC_CAL_VAL_EFUSE_VREF

#define DEFAULT_ENVELOPE_VALS    100
#define DEFAULT_LOW_PASS_VAL     20
#define DEFAULT_HIGH_PASS_VAL    20
#define DEFAULT_BLEND_VAL        1

#define BLEND_VAL_MIN            0
#define BLEND_VAL_MAX            255
#define BLEND_VAL_MAX_BITS       8

#define SQU_DURATION_WIDTH       8
#define SQU_DURATION_MAX         255

#define EXTRA_SPEED_INCREASE 35
#define EXTRA_SPEED_DECREASE 1
#define EXTRA_SPEED_MINIMUM 1

#define ANALOG_READ_LOOP_MS      8

#define ROTARY_HIGH_MIN          2900
#define ROTARY_DELTA             200
#define ROTARY_MIDHIGH_TARGET    2200
#define ROTARY_MIDLOW_TARGET     1089
#define ROTARY_LOW_MAX           700

#define MAX_VOLTAGE_READ         3300

#define MAX_MENU_STATE_VAL 4


// Always accessible values
extern menu_state menu_select;
extern int_fast16_t low_pass_val;

// ADSR menu values
extern int_fast16_t attack_val;
extern int_fast16_t decay_val;
extern int_fast16_t sustain_val;
extern int_fast16_t release_val;

// Wave menu values
extern wave_type wave_select1_val;
extern int_fast16_t wave_blend_val;
extern wave_type wave_select2_val;
extern int_fast16_t high_pass_val;

// Sequencer setup values
extern int_fast16_t squ_enable_val;
extern int_fast16_t squ_length_val;
extern int_fast16_t squ_tempo_val;
extern int_fast16_t squ_duration_val;


void task_adc();

void adc_init();

void ledc_init();

void gpio_init();


#endif // BASIC_IO_H_
