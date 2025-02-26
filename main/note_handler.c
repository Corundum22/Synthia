#include <stdint.h>
#include <stdio.h>
#include "note_handler.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "basic_io.h"
#include "data_y_splitter.h"
#include "sequencer.h"


note_data note_properties[NUM_VOICES + SEQ_VOICES] = {{
    is_pressed: false,
    is_sounding: false,
    envelope_state: nothing,
    note_num: 0,
    multiplier: MIN_ENVELOPE_VAL,
}};

esp_timer_handle_t envelope_timer_handle;


void set_keypress(uint_fast8_t key_num) {
    for (int i = 0; i < NUM_VOICES; i++) {
        if (note_properties[i].note_num == key_num && note_properties[i].is_pressed == true) {
            printf("Unison note rejected\n");
            return; // return without doing anything if the key is already pressed
                    // i.e. no unison keys
        }
    }
    for (int i = 0; i < NUM_VOICES; i++) {
        if (note_properties[i].is_pressed == false) {

            note_properties[i].is_pressed = true;
            note_properties[i].is_sounding = true;
            note_properties[i].envelope_state = attack;
            note_properties[i].note_num = key_num;

            printf("Note has been turned on\n");
            return;
        }
    }
}

void set_keyrelease(uint_fast8_t key_num) {
    for (int i = 0; i < NUM_VOICES; i++) {
        if (note_properties[i].note_num == key_num && note_properties[i].is_pressed == true) {
            note_properties[i].is_pressed = false;
            note_properties[i].envelope_state = release;
        }
    }
}


void set_squ_keypress(uint_fast8_t key_num) {
    for (int i = NUM_VOICES; i < NUM_VOICES + SEQ_VOICES; i++) {
        if (note_properties[i].note_num == key_num && note_properties[i].is_sounding == true) {
            printf("Unison note rejected\n");
            return; // return without doing anything if the key is already pressed
                    // i.e. no unison keys
        }
    }
    for (int i = NUM_VOICES; i < NUM_VOICES + SEQ_VOICES; i++) {
        if (note_properties[i].is_pressed == false) {

            note_properties[i].is_pressed = true;
            note_properties[i].is_sounding = true;
            note_properties[i].envelope_state = attack;
            note_properties[i].note_num = key_num;

            printf("Sequencer note has been turned on\n");
            return;
        }
    }
}

void set_squ_keyrelease(uint_fast8_t key_num) {
    for (int i = NUM_VOICES; i < NUM_VOICES + SEQ_VOICES; i++) {
        if (note_properties[i].note_num == key_num && note_properties[i].is_pressed == true) {
            note_properties[i].is_pressed = false;
            note_properties[i].envelope_state = release;
        }
    }
}

void envelope_timer_callback() {
    for (int i = 0; i < NUM_VOICES + SEQ_VOICES; i++) {
        if (note_properties[i].is_sounding) {
        switch (note_properties[i].envelope_state) {
            case sustain:
                // The note is divided by the smallest possible number,
                // making the output signal the loudest it can be
                note_properties[i].multiplier = DECAY_LEVEL;
                break;
            case attack:
                note_properties[i].multiplier += attack_nh;
                if (note_properties[i].multiplier >= MAX_ENVELOPE_VAL) {
                    note_properties[i].multiplier = MAX_ENVELOPE_VAL;
                    note_properties[i].envelope_state = decay;
                }
                break;
            case decay:
                note_properties[i].multiplier -= decay_nh;
                if (((int) note_properties[i].multiplier) <= DECAY_LEVEL) {
                    note_properties[i].multiplier = DECAY_LEVEL;
                    note_properties[i].envelope_state = sustain;
                }
                break;
            case release:
                int temp_val = (int) note_properties[i].multiplier - release_nh;
                if (temp_val < MIN_ENVELOPE_VAL) {
                    note_properties[i].multiplier = MIN_ENVELOPE_VAL;
                    note_properties[i].is_sounding = false;
                    note_properties[i].envelope_state = nothing;
                }
                note_properties[i].multiplier = temp_val;
                break;
            default:
                note_properties[i].envelope_state = nothing;
                note_properties[i].multiplier = MIN_ENVELOPE_VAL;
                note_properties[i].is_sounding = false;
                break;
        }
        }
    }
}

void envelope_timer_init() {
    const esp_timer_create_args_t envelope_timer_args = {
        .callback = &envelope_timer_callback,
        .name = "Envelope timer",
    };

    ESP_ERROR_CHECK(esp_timer_create(&envelope_timer_args, &envelope_timer_handle));
    ESP_ERROR_CHECK(esp_timer_start_periodic(envelope_timer_handle, ENVELOPE_TRIGGER_PERIOD_US));
}
