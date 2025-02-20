#ifndef DATA_Y_SPLITTER_
#define DATA_Y_SPLITTER_

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <stdint.h>


extern SemaphoreHandle_t guiSemaphore; // protects the copied data to be used by the gui
extern SemaphoreHandle_t ySplitterSemaphore; // protects the original data sources


// Note handler always accessible values
extern menu_state menu_select_nh;
extern int_fast16_t low_pass_nh;

// Note handler ADSR menu values
extern int_fast16_t attack_nh;
extern int_fast16_t decay_nh;
extern int_fast16_t sustain_nh;
extern int_fast16_t release_nh;

// Note handler wave menu values
extern int_fast16_t wave_select_nh;

// Note handler sequencer setup values
extern int_fast16_t sequencer_enable_nh;
extern int_fast16_t sequencer_clear_nh;

// Note handler sequencer page 1 note values
extern int_fast16_t squ_note_1_nh;
extern int_fast16_t squ_note_2_nh;
extern int_fast16_t squ_note_3_nh;
extern int_fast16_t squ_note_4_nh;

// Note handler sequencer page 2 note values
extern int_fast16_t squ_note_5_nh;
extern int_fast16_t squ_note_6_nh;
extern int_fast16_t squ_note_7_nh;
extern int_fast16_t squ_note_8_nh;


// GUI always accessible values
extern menu_state menu_select_gui;
extern int_fast16_t low_pass_gui;

// GUI ADSR menu values
extern int_fast16_t attack_gui;
extern int_fast16_t decay_gui;
extern int_fast16_t sustain_gui;
extern int_fast16_t release_gui;

// GUI wave menu values
extern int_fast16_t wave_select_gui;

// GUI sequencer setup values
extern int_fast16_t sequencer_enable_gui;
extern int_fast16_t sequencer_clear_gui;

// GUI sequencer page 1 note values
extern int_fast16_t squ_note_1_gui;
extern int_fast16_t squ_note_2_gui;
extern int_fast16_t squ_note_3_gui;
extern int_fast16_t squ_note_4_gui;

// GUI sequencer page 2 note values
extern int_fast16_t squ_note_5_gui;
extern int_fast16_t squ_note_6_gui;
extern int_fast16_t squ_note_7_gui;
extern int_fast16_t squ_note_8_gui;


void data_split_init();

void task_data_split();

#endif // DATA_Y_SPLITTER_
