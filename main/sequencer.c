//trying git again yayyy

#include <stdio.h>
#include <stdint.h>
#include <string.h>
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
uint_fast8_t test_pattern[SEQ_LEN] = { 0, 7, 14, 21, 28, 35, 
    42, 49, 56, 63, 70, 77, 84, 91, 98, 105, 112, 119, 126, 
    6, 13, 20, 27, 34, 41, 48, 55, 62, 69, 76, 83, 90, 97, 104, 
    111, 118, 125, 5, 12, 19, 26, 33, 40, 47, 54, 61, 68, 75, 
    82, 89, 96, 103, 110, 117, 124, 4, 11, 18, 25, 32, 39, 46, 
    53, 60 };

uint_fast8_t test_pattern_JOLT[SEQ_LEN] = { 
    56, 47, 52, 54, 59, 
    56, 47, 52, 54, 59, 
    56, 47, 52, 54, 59, 
    56, 47, 52, 54, 
    56, 54, 52, 47, 52, 54,
    56, 54, 52, 47, 52, 54,
    56, 54, 52, 47, 52, 54, 59,
    56, 47, 52, 54,
    56, 54, 52, 45, 52, 54,
    56, 54, 52, 45, 52, 54,
    56, 54, 52, 45, 49, 52, 54
};
uint_fast8_t test_pattern_HOTTOGO[SEQ_LEN] = { 
    35, 35, 46, 46, 47, 47, 46, 46,
    35, 35, 46, 46, 47, 47, 46, 46,
    35, 35, 46, 46, 47, 47, 46, 46,
    35, 35, 46, 46, 47, 47, 46, 46,
    39, 39, 50, 50, 51, 51, 50, 50,
    39, 39, 50, 50, 51, 51, 50, 50,
    39, 39, 50, 50, 51, 51, 50, 50,
    39, 39, 50, 50, 51, 51, 50, 50,
};

//fireflies pattern working on this
//Bb4, Bb5, D6, Bb4, Bb5, Bb6, Bb5, Eb6, 
//Eb5, Bb5, F5, Eb5, F5, Bb6, F5, Eb5,









esp_timer_handle_t sequencer_timer_handle;

int_fast8_t current_seq_index = 0;


void task_sequencer() {
    while (1) {
        // TODO: make this useful
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void sequencer_timer_callback() {
	// Execute if sequencer is enabled
    if (squ_enable_squ) {
        set_squ_keyrelease(test_pattern[(current_seq_index + SEQ_LEN - 1) % SEQ_LEN]);
        set_squ_keypress(test_pattern[current_seq_index]);

        current_seq_index++;

        if (current_seq_index >= squ_length_squ) {
            current_seq_index = 0;
        }
    }
}

void update_squ_timer(int_fast16_t new_val) {
    uint64_t us_new_val = BPM_US_FACTOR / (new_val * TEMPO_PERIOD_MULTIPLIER);
    ESP_ERROR_CHECK(esp_timer_restart(sequencer_timer_handle, us_new_val));
}

void sequencer_timer_init() {
    const esp_timer_create_args_t sequencer_timer_args = {
        .callback = &sequencer_timer_callback,
        .name = "Sequencer timer",
    };

    ESP_ERROR_CHECK(esp_timer_create(&sequencer_timer_args, &sequencer_timer_handle));
    ESP_ERROR_CHECK(esp_timer_start_periodic(sequencer_timer_handle, DEFAULT_SQU_TEMPO_US));
}
