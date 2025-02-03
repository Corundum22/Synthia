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

#define MENU_POT_1    ADC_CHANNEL_0 // gpio36
#define MENU_POT_2    ADC_CHANNEL_1 // gpio37
#define MENU_POT_3    ADC_CHANNEL_2 // gpio38
#define MENU_POT_4    ADC_CHANNEL_3 // gpio39
#define VOL_POT       ADC_CHANNEL_4 // gpio32
#define LOW_PASS_POT  ADC_CHANNEL_5 // gpio33

#define MENU_ATTEN    ADC_ATTEN_DB_12
#define MENU_BITS     ADC_BITWIDTH_11
#define MENU_UNIT     ADC_UNIT_1
#define DEFAULT_VREF  1100

#define CHOSEN_ADC_CALI_SCHEME   ESP_ADC_CAL_VAL_EFUSE_VREF


extern uint_fast16_t sin_volume;
extern uint_fast16_t sin_1;
extern uint_fast16_t sin_2;
extern uint_fast16_t sin_3;


void task_adc();

void adc_init();

void ledc_init();

void gpio_init();


#endif // BASIC_IO_H_
