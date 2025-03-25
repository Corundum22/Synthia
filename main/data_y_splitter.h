#ifndef DATA_Y_SPLITTER_
#define DATA_Y_SPLITTER_

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "global_header.h"
#include "note_handler.h"
#include "sequencer.h"


extern SemaphoreHandle_t guiSemaphore; // protects the copied data to be used by the gui
extern SemaphoreHandle_t ySplitterSemaphore; // protects the original data sources

// Note handler ADSR menu values
extern int_fast16_t attack_nh;
extern int_fast16_t decay_nh;
extern int_fast16_t sustain_nh;
extern int_fast16_t release_nh;

// Synth audio wave menu values
extern wave_type wave_select1_syn;
extern int_fast16_t wave_blend_syn;
extern wave_type wave_select2_syn;

// Not part of a menu
extern int_fast16_t wave_blend_pair_syn;

// Sequencer sequencer setup values
extern int_fast16_t squ_enable_squ;
extern int_fast16_t squ_length_squ;
extern int_fast16_t squ_tempo_squ;
extern int_fast16_t squ_duration_squ;

// GUI always accessible values
extern menu_state menu_select_gui;
extern int_fast16_t low_pass_gui;

// GUI ADSR menu values
extern int_fast16_t attack_gui;
extern int_fast16_t decay_gui;
extern int_fast16_t sustain_gui;
extern int_fast16_t release_gui;

// GUI wave menu values
extern wave_type wave_select1_gui;
extern int_fast16_t wave_blend_gui;
extern wave_type wave_select2_gui;
extern int_fast16_t high_pass_gui;

// GUI sequencer setup values
extern int_fast16_t squ_enable_gui;
extern int_fast16_t squ_length_gui;
extern int_fast16_t squ_tempo_gui;
extern int_fast16_t squ_duration_gui;

// GUI sequencer running values
extern int_fast16_t squ_index_gui;
extern int_fast16_t squ_program_index_gui;
extern uint_fast8_t squ_pattern_gui[];

// GUI note data
extern note_data note_properties_gui[NUM_VOICES + SEQ_VOICES];


void data_split_init();

void task_data_split();


#endif // DATA_Y_SPLITTER_
