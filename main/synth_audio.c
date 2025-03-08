#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "driver/dac_continuous.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "portmacro.h"
#include "waveform_tables.h"
#include "uart_handler.h"
#include "synth_audio.h"
#include "note_handler.h"
#include "global_header.h"
#include "sequencer.h"


#define NUM_COMB_VOICES (NUM_VOICES + SEQ_VOICES)

#define GET_NOTE(i) \
if (note_properties[i].is_sounding == true) {\
    data += wave(note_properties[i].note_num, note_properties[i].multiplier, time);\
    times_added++;\
}



dac_continuous_handle_t dac_handle;
QueueHandle_t dac_event_queue;


static inline int_fast32_t wave(uint_fast8_t midi_note_number, uint_fast16_t multiply_val, uint_fast32_t time) {
    uint_fast32_t ratio_numerator = ratio_num[midi_note_number];
    uint_fast32_t ratio_denominator = ratio_denom[midi_note_number];

    uint_fast32_t point_in_cycle = ((time * ratio_numerator) / ratio_denominator) & 0b11111111;

    int_fast32_t result = (int_fast32_t) sin_array[point_in_cycle];
    result = (result * multiply_val) >> MULTIPLIER_WIDTH;

    return result;
}

static inline uint16_t audio_sample_get(uint32_t time) {
    int16_t data = 0;
    uint_fast8_t times_added = 0;

    #if NUM_COMB_VOICES >= 1
        GET_NOTE(0)
    #endif
    #if NUM_COMB_VOICES >= 2
        GET_NOTE(1)
    #endif
    #if NUM_COMB_VOICES >= 3
        GET_NOTE(2)
    #endif
    #if NUM_COMB_VOICES >= 4
        GET_NOTE(3)
    #endif
    #if NUM_COMB_VOICES >= 5
        GET_NOTE(4)
    #endif
    #if NUM_COMB_VOICES >= 6
        GET_NOTE(5)
    #endif
    #if NUM_COMB_VOICES >= 7
        GET_NOTE(6)
    #endif
    #if NUM_COMB_VOICES >= 8
        GET_NOTE(7)
    #endif
    #if NUM_COMB_VOICES >= 9
        GET_NOTE(8)
    #endif
    #if NUM_COMB_VOICES >= 10
        GET_NOTE(9)
    #endif

    // TODO: uncomment to prevent clipping with multiple voices active
    //if (times_added != 0) data /= (NUM_VOICES + SEQ_VOICES);
    
    uint16_t unsigned_data = ((uint16_t) data) + 0b0111111111111111;

    return unsigned_data;
    //ESP_ERROR_CHECK(dac_oneshot_output_voltage(lower_dac_handle, (uint8_t) unsigned_data));
    //ESP_ERROR_CHECK(dac_oneshot_output_voltage(upper_dac_handle, (uint8_t) (unsigned_data >> 8)));
}

#define AUDIO_CHUNK_SIZE 256
#define NUM_DAC_CHANNELS 2
void task_audio_generate() {

    uint32_t time = 0;

    while (1) {

        dac_event_data_t event_data;
        size_t byte_written = 0;
        while (byte_written < AUDIO_CHUNK_SIZE) {
            xQueueReceive(dac_event_queue, &event_data, portMAX_DELAY);

 
            uint8_t data_array[AUDIO_CHUNK_SIZE * NUM_DAC_CHANNELS];
            for (uint_fast16_t i = 0; i < AUDIO_CHUNK_SIZE; i++) {
                uint16_t data = audio_sample_get(time);
                
                data_array[i * 2] = (uint8_t) data;
                data_array[(i * 2) + 1] = (uint8_t) (data >> 8);
            
                time++;
            }


            size_t loaded_bytes = 0;
            ESP_ERROR_CHECK(dac_continuous_write_asynchronously(dac_handle, event_data.buf, event_data.buf_size, data_array, AUDIO_CHUNK_SIZE * NUM_DAC_CHANNELS, &loaded_bytes));
            
            byte_written += loaded_bytes;
        }
    
        while (uxQueueSpacesAvailable(dac_event_queue) == 0) {
            vTaskDelay(pdMS_TO_TICKS(2));
        }

    }
}

static bool IRAM_ATTR dac_on_convert_done_callback(dac_continuous_handle_t handle, const dac_event_data_t *event, void *user_data) {
    QueueHandle_t user_data_queue = (QueueHandle_t) user_data;
    BaseType_t need_awoke;

    // Drop oldest item, when queue is full
    if (xQueueIsQueueFullFromISR(user_data_queue)) {
        dac_event_data_t dummy_data;
        xQueueReceiveFromISR(user_data_queue, &dummy_data, &need_awoke);
    }

    // Send the event from callback
    xQueueSendFromISR(user_data_queue, event, &need_awoke);
    return need_awoke;
}

void dac_init() {
    // Configure DAC for lower 8 bits of output signal
    dac_continuous_config_t dac_config = {
        .chan_mask = DAC_CHANNEL_MASK_ALL,
        .desc_num = 4, // TODO: select a proper value for this
        .buf_size = AUDIO_BUF_SIZE * NUM_DAC_CHANNELS,
        .freq_hz = OUTPUT_SAMPLE_RATE,
        .offset = 0, // TODO: use this instead of hardcoding an offset
        .clk_src = DAC_DIGI_CLK_SRC_APLL,
        .chan_mode = DAC_CHANNEL_MODE_SIMUL,
    };
    ESP_ERROR_CHECK(dac_continuous_new_channels(&dac_config, &dac_handle));


    dac_event_queue = xQueueCreate(10, sizeof(dac_event_data_t));
    dac_event_callbacks_t cbs = {
        .on_convert_done = dac_on_convert_done_callback,
        .on_stop = NULL,
    };
    ESP_ERROR_CHECK(dac_continuous_register_event_callback(dac_handle, &cbs, dac_event_queue));


    ESP_ERROR_CHECK(dac_continuous_enable(dac_handle));
    

    ESP_ERROR_CHECK(dac_continuous_start_async_writing(dac_handle));
}
