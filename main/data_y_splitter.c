#include <stdbool.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "note_handler.h"
#include "basic_io.h"
#include "data_y_splitter.h"
#include "freertos/task.h"
#include "global_header.h"
#include "sequencer.h"

SemaphoreHandle_t guiSemaphore; // protects the copied data to be used by the gui
SemaphoreHandle_t ySplitterSemaphore; // protects the original data sources


// Note handler ADSR menu values
int_fast16_t attack_nh = DEFAULT_ENVELOPE_VALS;
int_fast16_t decay_nh = DEFAULT_ENVELOPE_VALS;
int_fast16_t sustain_nh = DEFAULT_ENVELOPE_VALS;
int_fast16_t release_nh = DEFAULT_ENVELOPE_VALS;

// Synth audio wave menu values
int_fast16_t wave_select_syn = 1;

// Note handler sequencer setup values
int_fast16_t squ_enable_squ = 0;
int_fast16_t squ_length_squ = 0;
int_fast16_t squ_tempo_squ = 1;
int_fast16_t squ_duration_squ = 1;
int_fast16_t current_squ_index = 0;

// GUI always accessible values
menu_state menu_select_gui = madsr;
int_fast16_t low_pass_gui = DEFAULT_LOW_PASS_VAL;

// GUI ADSR menu values
int_fast16_t attack_gui = DEFAULT_ENVELOPE_VALS;
int_fast16_t decay_gui = DEFAULT_ENVELOPE_VALS;
int_fast16_t sustain_gui = DEFAULT_ENVELOPE_VALS;
int_fast16_t release_gui = DEFAULT_ENVELOPE_VALS;

// GUI wave menu values
int_fast16_t wave_select_gui = 1;
int_fast16_t high_pass_gui = DEFAULT_HIGH_PASS_VAL;

// GUI sequencer setup values
int_fast16_t squ_enable_gui = 0;
int_fast16_t squ_length_gui = 0;
int_fast16_t squ_tempo_gui = 1;
int_fast16_t squ_duration_gui = 1;
int_fast16_t squ_index_gui = 0;

// GUI note data
note_data note_properties_gui[NUM_VOICES + SEQ_VOICES] = {{
    is_pressed: false,
    is_sounding: false,
    envelope_state: nothing,
    note_num: 0,
    multiplier: MIN_ENVELOPE_VAL,
}};

static void copy_gui();
static void copy_squ();
static void copy_nh();
static void copy_syn();
static inline void note_data_deep_copy();



void data_split_init() {
    guiSemaphore = xSemaphoreCreateMutex();
    ySplitterSemaphore = xSemaphoreCreateBinary();
}

void task_data_split() {
    while (1) {
        if (pdTRUE == xSemaphoreTake(ySplitterSemaphore, portMAX_DELAY)) {
            copy_squ();
            copy_gui();
            copy_nh();
            copy_syn();
        }
    }
}

static void copy_gui() {
    xSemaphoreTake(guiSemaphore, portMAX_DELAY);

    menu_select_gui = menu_select;
    low_pass_gui = low_pass_val;
    high_pass_gui = high_pass_val;
    attack_gui = attack_val;
    decay_gui = decay_val;
    sustain_gui = sustain_val;
    release_gui = release_val;
    wave_select_gui = wave_select_val;
    squ_enable_gui = squ_enable_val;
    squ_length_gui = squ_length_val;
    squ_tempo_gui = squ_tempo_val;
    squ_duration_gui = squ_duration_val;
    note_data_deep_copy();

    squ_index_gui = current_squ_index;

    xSemaphoreGive(guiSemaphore);
}

static void copy_squ() {
    squ_enable_squ = squ_enable_val;
    squ_length_squ = squ_length_val;
    squ_tempo_squ = squ_tempo_val;
    squ_duration_squ = squ_duration_val;
}

static void copy_nh() {
    attack_nh = attack_val;
    decay_nh = decay_val;
    sustain_nh = sustain_val;
    release_nh = release_val;
}

static void copy_syn() {
    wave_select_syn = wave_select_val;
}

static inline void note_data_deep_copy() {
    for (int i = 0; i < NUM_VOICES + SEQ_VOICES; i++) {
        note_properties_gui[i] = note_properties_slow[i];
    }
}
