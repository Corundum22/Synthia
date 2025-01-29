#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/dac_oneshot.h"
#include "driver/gptimer.h"
#include "synth_audio.h"
#include "basic_io.h"
#include "uart_handler.h"


TaskHandle_t audio_task_handle;
TaskHandle_t exec_init_task_handle;
TaskHandle_t midi_uart_task_handle;


static void task_create() {
    xTaskCreate(task_audio_generate, "Audio Generation Task", 2048, NULL, 5, &audio_task_handle);
    xTaskCreate(task_midi_uart, "MIDI UART Task", 4096, NULL, 10, &midi_uart_task_handle);
}

static void exec_init() {
    timer_init();
    printf("Timer init finished\n");

    dac_init();
    printf("DAC init finished\n");

    gpio_init();
    printf("GPIO init finished\n");

    ledc_init();
    printf("LEDC init finished\n");

    uart_init();
    printf("UART init finished\n");

    task_create();
    printf("Tasks started\n");

    vTaskDelete(NULL);
}

void app_main(void) {
    printf("Program started\n");
    
    xTaskCreate(exec_init, "Max Priority Startup Task", 4096, NULL, configMAX_PRIORITIES - 1, &exec_init_task_handle);
}
