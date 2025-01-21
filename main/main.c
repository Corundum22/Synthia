#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/dac_oneshot.h"
#include "driver/gptimer.h"
#include "driver/gpio.h"
#include "synth_audio.h"


TaskHandle_t audio_task_handle;
extern uint16_t current_note;


static void IRAM_ATTR gpio_interrupt_handler(void *args) {
    int gpio_num = (int) args;
    switch (gpio_num) {
        case 12:
            current_note++;
            break;
        case 14:
            current_note--;
            break;
    }
}

static void gpio_init() {
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_POSEDGE,
        .pin_bit_mask = GPIO_INPUT_PIN_SEL,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 1,
    };
    gpio_config(&io_conf);

    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(FREQ_UP_BUTTON, gpio_interrupt_handler, (void *) FREQ_UP_BUTTON);
    gpio_isr_handler_add(FREQ_DOWN_BUTTON, gpio_interrupt_handler, (void *) FREQ_DOWN_BUTTON);
}

static void task_create() {
    xTaskCreate(task_audio_generate, "Audio Generation Task", 4096, NULL, 10, &audio_task_handle);
}

void app_main(void) {
    printf("Program started\n");

    timer_init();
    printf("Timer init finished\n");

    dac_init();
    printf("DAC init finished\n");

    gpio_init();
    printf("GPIO init finished\n");

    task_create();
}
