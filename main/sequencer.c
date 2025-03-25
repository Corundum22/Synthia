//trying git again yayyy

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "basic_io.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "driver/uart.h"
#include "freertos/projdefs.h"
#include "freertos/queue.h"
#include "uart_handler.h"
#include "synth_audio.h"
#include "note_handler.h"
#include "data_y_splitter.h"
#include "esp_timer.h"
#include "sequencer.h"


// test_pattern is a temporary pattern of midi note numbers
// intended only for testing sequencer_timer_callback
uint_fast8_t squ_pattern_weird[SEQ_LEN] = {     
    60, 60, 60, 60, 60, 
    65, 65, 65, 65, 65, 
    69, 69, 69, 69, 69, 
    60, 60, 60, 60, 60, 
    65, 65, 65, 65, 65, 
    69, 69, 69, 69, 69, 
    60, 60, 60, 60, 60, 
    65, 65, 65, 65, 65, 
    69, 69, 69, 69, 69, 
    60, 60, 60, 60, 60, 
    65, 65, 65, 65, 65, 
    69, 69, 69, 69, 69, 
    60, 60, 60, 60, 
    };

uint_fast8_t squ_basic_pattern[SEQ_LEN] = {     
    50, 50, 50, 50, 50, 
    50, 50, 50, 50, 50, 
    50, 50, 50, 50, 50, 
    50, 50, 50, 50, 
    50, 50, 50, 50, 50, 50,
    50, 50, 50, 50, 50, 50,
    50, 50, 50, 50, 50, 50, 50,
    50, 50, 50, 50, 
    50, 50, 50, 50, 50, 50,
    50, 50, 50, 50, 50, 50,
    50, 50, 50, 50, 50, 50, 50,
    };

//uint_fast8_t test_pattern_JOLT[SEQ_LEN] = { 
int_fast8_t squ_pattern[SEQ_LEN] = { 
    80, 71, 76, 78, 83, 
    80, 71, 76, 78, 83, 
    80, 71, 76, 78, 83, 
    80, 71, 76, 78, 
    80, 78, 76, 71, 76, 78, 
    80, 78, 76, 71, 76, 78, 
    80, 78, 76, 71, 76, 78, 83, 
    80, 71, 76, 78, 
    80, 78, 76, 69, 76, 78, 
    80, 78, 76, 69, 76, 78, 
    80, 78, 76, 69, 73, 76, 78, 
};
uint_fast8_t test_pattern_HOTTOGO[SEQ_LEN] = { 
    35, 35, 46, 46, 47, 47, 46, 46,
    35, 35, 46, 46, 47, 47, 46, 46,
    35, 35, 46, 46, 47, 47, 46, 46,
    35, 35, 46, 46, 47, 47, 46, 46,
    39, 39, 50, 50, 51, 51, 50, 50,
    39, 39, 50, 50, 51, 51, 50, 50,
    39, 39, 50, 50, 51, 51, 50, 50,
    39, 39, 50, 50, 51, 51, 50, 50
};

//fireflies pattern working on this
//Bb4, Bb5, D6, Bb4, Bb5, Bb6, Bb5, Eb6, 
//Eb5, Bb5, F5, Eb5, F5, Bb6, F5, Eb5,









esp_timer_handle_t sequencer_timer_handle;

int_fast8_t squ_index = 0;
int_fast8_t squ_program_index = 0;

bool next_is_on_squ_press = true;

void sequencer_timer_callback() {
	// Execute if sequencer is enabled
    if (squ_enable_squ) {
        if (next_is_on_squ_press) {

            set_squ_keypress(squ_pattern[squ_index]);

        } else {

            set_squ_keyrelease(squ_pattern[squ_index]);

            squ_index++;
            if (squ_index >= squ_length_squ) squ_index = 0;

        }

    }
}

void program_sequencer(uint_fast8_t key_num){
    if(squ_enable_squ == 0) {
        squ_pattern[squ_program_index] = key_num;

        squ_program_index++;

        if (squ_program_index >= squ_length_squ) {
            squ_program_index = 0;
        }
    }

}

inline static int_fast16_t get_squ_duration_sided() {
    return (next_is_on_squ_press) ? squ_duration_squ : (SQU_DURATION_MAX - squ_duration_squ);
}

void update_squ_timer(int_fast16_t new_val) {
    int_fast16_t squ_duration_sided = get_squ_duration_sided();

    uint64_t us_new_val = ((BPM_US_FACTOR / (new_val * TEMPO_PERIOD_MULTIPLIER)) * squ_duration_sided) >> SQU_DURATION_WIDTH;
    ESP_ERROR_CHECK(esp_timer_restart(sequencer_timer_handle, us_new_val));
}

void pause_squ_timer() {
    ESP_ERROR_CHECK(esp_timer_stop(sequencer_timer_handle));
    set_squ_keyrelease(squ_pattern[(squ_index + SEQ_LEN - 1) % SEQ_LEN]);
}

void resume_squ_timer(int_fast16_t us_val) {
    int_fast16_t squ_duration_sided = get_squ_duration_sided();

    uint64_t us_new_val = ((BPM_US_FACTOR / (us_val * TEMPO_PERIOD_MULTIPLIER)) * squ_duration_sided) >> SQU_DURATION_WIDTH;
    ESP_ERROR_CHECK(esp_timer_start_periodic(sequencer_timer_handle, us_new_val));
}

void sequencer_timer_init() {
    const esp_timer_create_args_t sequencer_timer_args = {
        .callback = &sequencer_timer_callback,
        .name = "Sequencer timer",
    };

    ESP_ERROR_CHECK(esp_timer_create(&sequencer_timer_args, &sequencer_timer_handle));
}
