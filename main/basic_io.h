#ifndef BASIC_IO_H_
#define BASIC_IO_H_

#define MIDI_PANIC_BUTTON       12
#define GPIO_INPUT_PIN_SEL      (1ULL<<MIDI_PANIC_BUTTON)
#define ESP_INTR_FLAG_DEFAULT   0

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_7_BIT // Set duty resolution to 7 bits
#define LEDC_FREQUENCY          312500 // Set frequency at 312.5 kHz


void ledc_init();

void IRAM_ATTR gpio_interrupt_handler(void *args);

void gpio_init();


#endif // BASIC_IO_H_
