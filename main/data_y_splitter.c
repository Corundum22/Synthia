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

SemaphoreHandle_t guiSemaphore; // protects the copied data to be used by the gui
SemaphoreHandle_t ySplitterSemaphore; // protects the original data sources


// Note handler always accessible values
menu_state menu_select_squ = madsr;
int_fast16_t low_pass_squ = DEFAULT_LOW_PASS_VAL;

// Note handler ADSR menu values
int_fast16_t attack_squ = DEFAULT_ENVELOPE_VALS;
int_fast16_t decay_squ = DEFAULT_ENVELOPE_VALS;
int_fast16_t sustain_squ = DEFAULT_ENVELOPE_VALS;
int_fast16_t release_squ = DEFAULT_ENVELOPE_VALS;

// Note handler wave menu values
int_fast16_t wave_select_squ = 1;

// Note handler sequencer setup values
int_fast16_t sequencer_enable_squ = 0;
int_fast16_t sequencer_clear_squ = 0;

// Note handler sequencer page 1 note values
int_fast16_t squ_note_1_squ = 0;
int_fast16_t squ_note_2_squ = 0;
int_fast16_t squ_note_3_squ = 0;
int_fast16_t squ_note_4_squ = 0;

// Note handler sequencer page 2 note values
int_fast16_t squ_note_5_squ = 0;
int_fast16_t squ_note_6_squ = 0;
int_fast16_t squ_note_7_squ = 0;
int_fast16_t squ_note_8_squ = 0;


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

// GUI sequencer setup values
int_fast16_t sequencer_enable_gui = 0;
int_fast16_t sequencer_clear_gui = 0;

// GUI sequencer page 1 note values
// tempo select
int_fast16_t squ_note_1_gui = 0;
// pattern length
int_fast16_t squ_note_2_gui = 0;
// note length
int_fast16_t squ_note_3_gui = 0;
int_fast16_t squ_note_4_gui = 0;

// GUI sequencer page 2 note values
int_fast16_t squ_note_5_gui = 0;
int_fast16_t squ_note_6_gui = 0;
int_fast16_t squ_note_7_gui = 0;
int_fast16_t squ_note_8_gui = 0;

static void copy_gui();
static void copy_squ();



void data_split_init() {
    guiSemaphore = xSemaphoreCreateMutex();
    ySplitterSemaphore = xSemaphoreCreateBinary();
}

void task_data_split() {
    while (1) {
        if (pdTRUE == xSemaphoreTake(ySplitterSemaphore, portMAX_DELAY)) {
            copy_squ();
            copy_gui();
        }
    }
}

static void copy_gui() {
    xSemaphoreTake(guiSemaphore, portMAX_DELAY);

    menu_select_gui = menu_select;
    low_pass_gui = low_pass_val;
    attack_gui = attack_val;
    decay_gui = decay_val;
    sustain_gui = sustain_val;
    release_gui = release_val;
    wave_select_gui = wave_select_val;
    sequencer_enable_gui = sequencer_enable_val;
    sequencer_clear_gui = sequencer_clear_val;
    squ_note_1_gui = squ_note_1_val;
    squ_note_2_gui = squ_note_2_val;
    squ_note_3_gui = squ_note_3_val;
    squ_note_4_gui = squ_note_4_val;
    squ_note_5_gui = squ_note_5_val;
    squ_note_6_gui = squ_note_6_val;
    squ_note_7_gui = squ_note_7_val;
    squ_note_8_gui = squ_note_8_val;

    xSemaphoreGive(guiSemaphore);
}

static void copy_squ() {
    menu_select_squ = menu_select;
    low_pass_squ = low_pass_val;
    attack_squ = attack_val;
    decay_squ = decay_val;
    sustain_squ = sustain_val;
    release_squ = release_val;
    wave_select_squ = wave_select_val;
    sequencer_enable_squ = sequencer_enable_val;
    sequencer_clear_squ = sequencer_clear_val;
    squ_note_1_squ = squ_note_1_val;
    squ_note_2_squ = squ_note_2_val;
    squ_note_3_squ = squ_note_3_val;
    squ_note_4_squ = squ_note_4_val;
    squ_note_5_squ = squ_note_5_val;
    squ_note_6_squ = squ_note_6_val;
    squ_note_7_squ = squ_note_7_val;
    squ_note_8_squ = squ_note_8_val;
}
