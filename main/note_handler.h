#ifndef NOTE_HANDLER_H_
#define NOTE_HANDLER_H_


#include <stdbool.h>
#include <stdint.h>
#include "global_header.h"
#include "sequencer.h"

#define NUM_VOICES                    8
#define ENVELOPE_TRIGGER_PERIOD_US    10000

#define MAX_ENVELOPE_VAL    0xFF
#define MIN_ENVELOPE_VAL    0x00
#define LOW_ENVELOPE_VAL    0x01

#define DECAY_LEVEL         170

#define MULTIPLIER_WIDTH    8


extern note_data note_properties[NUM_VOICES + SEQ_VOICES];
extern note_data note_properties_slow[NUM_VOICES + SEQ_VOICES];

void set_keypress(uint_fast8_t key_num);
void set_keyrelease(uint_fast8_t key_num);
void set_squ_keypress(uint_fast8_t key_num);
void set_squ_keyrelease(uint_fast8_t key_num);
void envelope_timer_init();


#endif
