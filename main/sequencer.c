//trying git again yayyy

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "driver/uart.h"
#include "freertos/queue.h"
#include "uart_handler.h"
#include "synth_audio.h"
#include "note_handler.h"
#include "data_y_splitter.h"


esp_timer_handle_t sequencer_timer_handle;







//squ_note_1_squ
//tempo knob 1
//squ_note_2_squ
//pattern length will be knob 2

//note length will be knob 3



//sequencer_enable_squ

//sequencer_clear_squ

note_data sequencer_values[SEQ_LENGTH] = {{
    is_pressed: false,
    is_sounding: false,
    envelope_state: nothing,
    note_num: 0,
    multiplier: MIN_ENVELOPE_VAL,
}};

int last_tempo = 0;
int current_seq_pos = 0;


void sequencer_timer_callback(){
    if(sequencer_enable_squ){
		//sequencer is enabled
        tempo = squ_note_1_squ*TEMPO_PERIOD_MULTIPLIER;
        tempo_period = 60000000/(tempo);
        
        
        
        if(last_tempo != tempo){
            esp_timer_stop(sequencer_timer_handle);
            esp_timer_start_periodic(sequencer_timer_handle, tempo_period);
        }




        current_seq_pos++;
        if(current_seq_pos > squ_note_2_squ){
            current_seq_pos = 0;
        }
	}


	if(sequencer_clear_squ){
			//clear sequencer
    }
}



void sequencer_timer_init() {
    const esp_timer_create_args_t sequencer_timer_args = {
        .callback = &sequencer_timer_callback,
        .name = "Sequencer timer",
    };

    ESP_ERROR_CHECK(esp_timer_create(&sequencer_timer_args, &sequencer_timer_handle));
    ESP_ERROR_CHECK(esp_timer_start_periodic(sequencer_timer_handle, tempo_period));
}





//set_keypress(uint_fast8_t key_num)

//set_keyrelease(uint_fast8_t key_num)



