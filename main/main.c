#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/dac_oneshot.h"
#include "driver/gptimer.h"
#include "sequencer.h"
#include "synth_audio.h"
#include "basic_io.h"
#include "uart_handler.h"
#include "display_control.h"
#include "note_handler.h"
#include "data_y_splitter.h"
#include "esp_task_wdt.h"


TaskHandle_t audio_task_handle;
TaskHandle_t exec_init_task_handle;
TaskHandle_t midi_uart_task_handle;
TaskHandle_t adc_task_handle;
TaskHandle_t display_task_handle;
TaskHandle_t data_split_task_handle;


static void task_create() {
    xTaskCreate(task_data_split, "Data splitting task", 2048, NULL, 11, &data_split_task_handle);
    xTaskCreate(task_audio_generate, "Audio Generation Task", 4096, NULL, 5, &audio_task_handle);
    xTaskCreate(task_midi_uart, "MIDI UART Task", 4096, NULL, 9, &midi_uart_task_handle);
    xTaskCreate(task_adc, "Potentiometer Checking Task", 1024, NULL, 7, &adc_task_handle);
    xTaskCreate(task_display, "Display Control Task", 4096*2, NULL, 3, &display_task_handle);
}

static void exec_init() {
    sequencer_timer_init();
    printf("Sequencer timer init finished\n");

    dac_init();
    printf("DAC init finished\n");

    gpio_init();
    printf("GPIO init finished\n");

    ledc_init();
    printf("LEDC init finished\n");

    uart_init();
    printf("UART init finished\n");

    adc_init();
    printf("ADC init finished\n");

    data_split_init();
    printf("Data split init finished\n");

    spi_init();
    printf("SPI initialized\n");

    task_create();
    printf("Tasks started\n");

    /*esp_task_wdt_config_t wdt_config = {
        .timeout_ms = 500,
        .idle_core_mask = (1 << 1) | (1 << 0),
        .trigger_panic = true,
    };
    esp_task_wdt_init(&wdt_config);*/
    //esp_task_wdt_add(midi_uart_task_handle);

    vTaskDelete(NULL);
}

void app_main(void) {
    printf("Program started\n");
    
    xTaskCreate(exec_init, "Max Priority Startup Task", 4096, NULL, configMAX_PRIORITIES - 1, &exec_init_task_handle);
}
