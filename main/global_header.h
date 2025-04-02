#ifndef GLOBAL_HEADER_H_
#define GLOBAL_HEADER_H_


#include <stdbool.h>


#define MAX_WAVE_NUMBER    3
enum wave_type_t {
    ssin = 0,
    striangle = 1,
    ssawtooth = 2,
    ssquare = 3,
} typedef wave_type;

enum menu_state_t {
    madsr = 0,
    mwave = 1,
    msequencer_setup = 2,
} typedef menu_state;

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

enum button_states_t {
    bhard,
    bsoft,
    bnothing,
} typedef button_states;

#endif
