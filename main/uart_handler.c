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


enum midi_event {
    waiting,
    note_on_key_num,
    note_on_velocity,
    note_off_key_num,
    note_off_velocity,
    off_waiting,
    on_waiting,
};


enum midi_event current_midi_event = waiting;
uint_fast8_t channel_num = 0;
uint_fast8_t velocity = 0;
uint_fast8_t key_num = 0;

extern note_data note_properties[];

QueueHandle_t midi_uart_queue;


void uart_init() {
    uart_config_t midi_uart_conf = {
        .baud_rate = MIDI_UART_BAUD_RATE,
        .data_bits = MIDI_UART_DATA_BITS,
        .parity = MIDI_UART_PARITY,
        .stop_bits = MIDI_UART_STOP_BITS,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2, MIDI_UART_BUFFER_SIZE * 2, 0, 100, &midi_uart_queue, 0));
    //printf("uart driver installed\n");
    ESP_ERROR_CHECK(uart_param_config(MIDI_UART_NUM, &midi_uart_conf));
    //printf("uart paramaters configured\n");
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, -1, 16, 19, 18));
                                // uart num, tx, rx, rts, cts
    //printf("uart pin set\n");

    ESP_ERROR_CHECK(uart_enable_rx_intr(UART_NUM_2));
    //printf("uart rx interrupts enabled\n");
}
int times_triggered = 0;
void task_midi_uart(void *pvParameters) {
    uart_event_t event;
    uint8_t *dtmp = (uint8_t *) malloc(MIDI_UART_BUFFER_SIZE + 1);
    while (1) {
        if (xQueueReceive(midi_uart_queue, (void *) &event, (TickType_t) portMAX_DELAY)) {
            bzero(dtmp, MIDI_UART_BUFFER_SIZE);
            //printf("times triggered = %d\n", ++times_triggered);
            
            switch (event.type) {

                case UART_DATA:
                    uart_read_bytes(MIDI_UART_NUM, dtmp, event.size, portMAX_DELAY);
                    if (dtmp[0] != 0xfe || dtmp[1] != 0x00 || dtmp[2] != 0x00)
                        printf("Data received: %2x%2x%2x\n", dtmp[0], dtmp[1], dtmp[2]);

                    for (int i = 0; i < event.size; i++) {

                        switch (current_midi_event) {
                            case on_waiting:
                            case off_waiting:
                            case waiting:
                                //printf("in waiting\n\n");

                                switch (dtmp[i] & 0b11110000) {
                                    case (NOTE_ON | STATUS_BIT):
                                        current_midi_event = note_on_key_num;
                                        break;
                                    case (NOTE_OFF | STATUS_BIT):
                                        current_midi_event = note_off_key_num;
                                        break;
                                    case (SYSTEM_MESSAGE | STATUS_BIT):
                                        goto not_here;
                                    default:
                                        break;
                                }

                                if (current_midi_event == on_waiting && dtmp[i] != 0) {
                                    goto note_on_midway;
                                }
                                if (current_midi_event == off_waiting && dtmp[i] != 0) {
                                    key_num = dtmp[i];
                                    goto note_off_completion_post_vel;
                                }
                                //if (current_midi_event == off_waiting || current_midi_event == on_waiting) current_midi_event = waiting;

                                break;

                            case note_on_key_num:
                                //printf("in on key num\n\n");
                                note_on_midway:
                                key_num = dtmp[i];
                                current_midi_event = note_on_velocity;
                                break;

                            case note_off_key_num:
                                //printf("in off key num\n\n");
                                key_num = dtmp[i];
                                current_midi_event = note_off_velocity;
                                break;

                            case note_on_velocity:
                                velocity = dtmp[i];
                                //printf("in on velocity %d\n\n", velocity);
                                current_midi_event = on_waiting;
                                if (velocity != 0) {
                                    set_keypress(key_num);
                                } else {
                                    set_keyrelease(key_num);
                                }
                                break;
                            
                            case note_off_velocity:
                                //printf("in off velocity\n\n");
                                velocity = dtmp[i];
                                note_off_completion_post_vel:
                                current_midi_event = off_waiting;
                                set_keyrelease(key_num);
                                break;
                        }
                    }
                    uart_flush_input(MIDI_UART_NUM);
                    xQueueReset(midi_uart_queue);

                    break;

                case UART_FIFO_OVF:
                    uart_flush_input(MIDI_UART_NUM);
                    xQueueReset(midi_uart_queue);
                    break;

                case UART_BUFFER_FULL:
                    uart_flush_input(MIDI_UART_NUM);
                    xQueueReset(midi_uart_queue);
                    break;

                case UART_BREAK:
                    //printf("MIDI: connection broken!\n");
                    break;

                case UART_PARITY_ERR:
                    //printf("MIDI: parity error!\n");
                    break;

                case UART_FRAME_ERR:
                    //printf("MIDI: frame error!\n");
                    break;

                default:
                    uart_flush_input(MIDI_UART_NUM);
                    xQueueReset(midi_uart_queue);
                    //printf("MIDI: a useless event occurred\n");
                    break;
            }
            not_here:
        }
    }
    free(dtmp);
    dtmp = NULL;
    //printf("MIDI task ended!");
    vTaskDelete(NULL);
}
