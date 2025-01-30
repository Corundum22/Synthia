#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "driver/uart.h"
#include "freertos/queue.h"
#include "uart_handler.h"


uint_fast8_t on_notes[NUM_VOICES] = {0};

QueueHandle_t midi_uart_queue;


static void set_on_note(uint_fast8_t key_num) {
    for (int i = 0; i < NUM_VOICES; i++) {
        if (on_notes[i] != 0) {
            on_notes[i] = key_num;
            return;
        }
    }
}

static void unset_on_note(uint_fast8_t key_num) {
    for (int i = 0; i < NUM_VOICES; i++) {
        if (on_notes[i] == key_num) {
            on_notes[i] = 0;
            return;
        }
    }
}


void uart_init() {
    uart_config_t midi_uart_conf = {
        .baud_rate = MIDI_UART_BAUD_RATE,
        .data_bits = MIDI_UART_DATA_BITS,
        .parity = MIDI_UART_PARITY,
        .stop_bits = MIDI_UART_STOP_BITS,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2, MIDI_UART_BUFFER_SIZE * 2, 0, 20, &midi_uart_queue, 0));
    printf("uart driver installed\n");
    ESP_ERROR_CHECK(uart_param_config(MIDI_UART_NUM, &midi_uart_conf));
    printf("uart paramaters configured\n");
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, 17, 16, 19, 18));
                                // uart num, tx, rx, rts, cts
    printf("uart pin set\n");

    ESP_ERROR_CHECK(uart_enable_rx_intr(UART_NUM_2));
    printf("uart rx interrupts enabled\n");
}

void task_midi_uart(void *pvParameters) {
    uart_event_t event;
    uint8_t *dtmp = (uint8_t *) malloc(MIDI_UART_BUFFER_SIZE + 1);
    while (1) {
        if (xQueueReceive(midi_uart_queue, (void *) &event, (TickType_t) portMAX_DELAY)) {
            bzero(dtmp, MIDI_UART_BUFFER_SIZE);
            
            switch (event.type) {

                case UART_DATA:
                    uart_read_bytes(MIDI_UART_NUM, dtmp, event.size, portMAX_DELAY);
                    printf("Data received: %s\n", dtmp);

                    uint_fast8_t channel_num = 0;
                    uint_fast8_t velocity = 0;
                    uint_fast8_t key_num = 0;
                    for (int i = 0; dtmp[i] != 0; i++) {
                        if (dtmp[i] & 0b10000000) {
                            channel_num = dtmp[i] & 0b00001111;

                            switch (dtmp[i] & 0b01110000) {
                                case NOTE_ON:
                                    i++;
                                    key_num = dtmp[i];
                                    i++;
                                    velocity = dtmp[i];
                                    set_on_note(key_num);
                                    printf("Note on received!\n");
                                    break;
                                case NOTE_OFF:
                                    i++;
                                    key_num = dtmp[i];
                                    i++;
                                    unset_on_note(key_num);
                                    printf("Note off received!\n");
                                    break;
                                default:
                                    break;
                            }
                        }
                    }

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
                    printf("MIDI: connection broken!\n");
                    break;

                case UART_PARITY_ERR:
                    printf("MIDI: parity error!\n");
                    break;

                case UART_FRAME_ERR:
                    printf("MIDI: frame error!\n");
                    break;

                default:
                    printf("MIDI: a useless event occurred\n");
                    break;
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}
