#ifndef GLOBAL_HEADER_H_
#define GLOBAL_HEADER_H_


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

#endif
