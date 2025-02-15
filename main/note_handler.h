#ifndef NOTE_HANDLER_H_
#define NOTE_HANDLER_H_


#include <stdbool.h>
#include <stdint.h>

#define NUM_VOICES                    8
#define ENVELOPE_TRIGGER_PERIOD_US    10000

#define MAX_ENVELOPE_VAL    0xFF
#define MIN_ENVELOPE_VAL    0x01

#define DECAY_LEVEL         170

#define MULTIPLIER_WIDTH    8


enum envelope_states_t {
    nothing,
    attack,
    decay,
    sustain,
    release,
} typedef envelope_states;

struct note_data_t {
    bool is_pressed;
    bool is_sounding;
    envelope_states envelope_state;
    uint_fast8_t note_num;
    int_fast16_t multiplier;
} typedef note_data;

extern note_data note_properties[NUM_VOICES];

void set_keypress(uint_fast8_t key_num);
void set_keyrelease(uint_fast8_t key_num);
void envelope_timer_init();


#endif
