#include <stdbool.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "note_handler.h"
#include "basic_io.h"
#include "data_y_splitter.h"
#include "freertos/task.h"

SemaphoreHandle_t guiSemaphore; // protects the copied data to be used by the gui
SemaphoreHandle_t ySplitterSemaphore; // protects the original data sources


// Note handler always accessible values
menu_state menu_select_nh = madsr;
int_fast16_t low_pass_nh = DEFAULT_LOW_PASS_VAL;

// Note handler ADSR menu values
int_fast16_t attack_nh = DEFAULT_ENVELOPE_VALS;
int_fast16_t decay_nh = DEFAULT_ENVELOPE_VALS;
int_fast16_t sustain_nh = DEFAULT_ENVELOPE_VALS;
int_fast16_t release_nh = DEFAULT_ENVELOPE_VALS;

// Note handler wave menu values
int_fast16_t wave_select_nh = 1;

// Note handler sequencer setup values
int_fast16_t sequencer_enable_nh = 0;
int_fast16_t sequencer_clear_nh = 0;

// Note handler sequencer page 1 note values
int_fast16_t squ_note_1_nh = 0;
int_fast16_t squ_note_2_nh = 0;
int_fast16_t squ_note_3_nh = 0;
int_fast16_t squ_note_4_nh = 0;

// Note handler sequencer page 2 note values
int_fast16_t squ_note_5_nh = 0;
int_fast16_t squ_note_6_nh = 0;
int_fast16_t squ_note_7_nh = 0;
int_fast16_t squ_note_8_nh = 0;


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
int_fast16_t squ_note_1_gui = 0;
int_fast16_t squ_note_2_gui = 0;
int_fast16_t squ_note_3_gui = 0;
int_fast16_t squ_note_4_gui = 0;

// GUI sequencer page 2 note values
int_fast16_t squ_note_5_gui = 0;
int_fast16_t squ_note_6_gui = 0;
int_fast16_t squ_note_7_gui = 0;
int_fast16_t squ_note_8_gui = 0;

static void copy_gui();
static void copy_nh();



void data_split_init() {
    guiSemaphore = xSemaphoreCreateMutex();
    ySplitterSemaphore = xSemaphoreCreateBinary();
}

void task_data_split() {
    while (1) {
        if (pdTRUE == xSemaphoreTake(ySplitterSemaphore, portMAX_DELAY)) {
            copy_nh();
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

static void copy_nh() {
    menu_select_nh = menu_select;
    low_pass_nh = low_pass_val;
    attack_nh = attack_val;
    decay_nh = decay_val;
    sustain_nh = sustain_val;
    release_nh = release_val;
    wave_select_nh = wave_select_val;
    sequencer_enable_nh = sequencer_enable_val;
    sequencer_clear_nh = sequencer_clear_val;
    squ_note_1_nh = squ_note_1_val;
    squ_note_2_nh = squ_note_2_val;
    squ_note_3_nh = squ_note_3_val;
    squ_note_4_nh = squ_note_4_val;
    squ_note_5_nh = squ_note_5_val;
    squ_note_6_nh = squ_note_6_val;
    squ_note_7_nh = squ_note_7_val;
    squ_note_8_nh = squ_note_8_val;
}
