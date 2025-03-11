#include <stdint.h>
#include <stdio.h>
#include "note_handler.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "basic_io.h"
#include "data_y_splitter.h"
#include "freertos/idf_additions.h"
#include "global_header.h"
#include "portmacro.h"
#include "sequencer.h"


note_data note_properties[NUM_VOICES + SEQ_VOICES] = {{
    is_pressed: false,
    is_sounding: false,
    envelope_state: nothing,
    note_num: 0,
    multiplier: MIN_ENVELOPE_VAL,
}};

// Duplicate of note_properties that is set in the envelope callback
// for use in the y-splitter
note_data note_properties_slow[NUM_VOICES + SEQ_VOICES] = {{
    is_pressed: false,
    is_sounding: false,
    envelope_state: nothing,
    note_num: 0,
    multiplier: MIN_ENVELOPE_VAL,
}};

esp_timer_handle_t envelope_timer_handle;

void program_sequencer(uint_fast8_t key_num){

//I'm gonna be honest I have no idea how getting data between stuff works...
//are there going to be issues if I try to access the sequencer data from here? IDK!!!!!!!! AT ALL!!!!
    if(squ_enable_squ == 0) {
        squ_pattern[current_squ_index] = key_num;
        current_squ_index++;
        if (current_squ_index >= squ_length_squ) {
            current_squ_index = 0;
        }
    }
}


void set_keypress(uint_fast8_t key_num) {

    // Prevent unison keypresses from retriggering a note
    for (int i = 0; i < NUM_VOICES; i++) {
        if (note_properties[i].note_num == key_num && note_properties[i].is_pressed == true) {
            return; // return without doing anything if the key is already pressed
                    // i.e. no unison keys
        }
    }

    // Trigger the note so long as there is at least one voice not being pressed
    for (int i = 0; i < NUM_VOICES; i++) {
        if (note_properties[i].is_pressed == false) {

            note_properties[i].is_pressed = true;
            note_properties[i].is_sounding = true;
            note_properties[i].envelope_state = attack;
            note_properties[i].note_num = key_num;


            // Program the sequencer
            program_sequencer(key_num);


            return;
        }
    }
}

void set_keyrelease(uint_fast8_t key_num) {
    for (int i = 0; i < NUM_VOICES; i++) {
        if (note_properties[i].note_num == key_num && note_properties[i].is_pressed == true) {
            note_properties[i].is_pressed = false;
            note_properties[i].envelope_state = release;
            note_properties[i].multiplier = MIN_ENVELOPE_VAL;
        }
    }
}


void set_squ_keypress(uint_fast8_t key_num) {
    for (int i = NUM_VOICES; i < NUM_VOICES + SEQ_VOICES; i++) {
        if (note_properties[i].note_num == key_num && note_properties[i].is_sounding == true) {
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
                    note_properties[i].multiplier = sustain_nh;
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
                    if (temp_val < LOW_ENVELOPE_VAL) {
                        note_properties[i].multiplier = LOW_ENVELOPE_VAL;
                        note_properties[i].is_sounding = false;
                        note_properties[i].envelope_state = nothing;
                    }
                    note_properties[i].multiplier = temp_val;
                    break;
                default:
                    note_properties[i].envelope_state = nothing;
                    note_properties[i].multiplier = LOW_ENVELOPE_VAL;
                    note_properties[i].is_sounding = false;
                    break;
            }
            //if (pdTRUE == xSemaphoreTake(guiSemaphore, portMAX_DELAY)) {
                note_properties_slow[i] = note_properties[i];
    /*for (int i = 0; i < NUM_VOICES + SEQ_VOICES; i++) {
        note_properties_gui[i].is_sounding = note_properties[i].is_sounding;
        note_properties_gui[i].is_pressed = note_properties[i].is_pressed;
        note_properties_gui[i].note_num = note_properties[i].note_num;
        note_properties_gui[i].envelope_state = note_properties[i].envelope_state;
        note_properties_gui[i].multiplier = note_properties[i].multiplier;
    }*/
                xSemaphoreGive(ySplitterSemaphore);
                //xSemaphoreGive(guiSemaphore);
            //}
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
