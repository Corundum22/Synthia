#ifndef BASIC_IO_H_
#define BASIC_IO_H_


#define MIDI_PANIC_BUTTON       13
#define GPIO_INPUT_PIN_SEL      (1ULL<<MIDI_PANIC_BUTTON)
#define ESP_INTR_FLAG_DEFAULT   0

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_DUTY_RES           LEDC_TIMER_7_BIT // Set duty resolution to 7 bits
#define LEDC_FREQUENCY          312500 // Set frequency at 312.5 kHz

#define STATUS_LEDC_CHANNEL       LEDC_CHANNEL_0
#define HIGH_PASS_LEDC_CHANNEL    LEDC_CHANNEL_1
#define LOW_PASS_LEDC_CHANNEL     LEDC_CHANNEL_2
#define DISTORTION_LEDC_CHANNEL   LEDC_CHANNEL_3

#define STATUS_LEDC       2
#define HIGH_PASS_LEDC    12
#define LOW_PASS_LEDC     14
#define DISTORTION_LEDC   27

#define MENU_POT_1    ADC_CHANNEL_0 // gpio36
#define MENU_POT_2    ADC_CHANNEL_1 // gpio37
#define MENU_POT_3    ADC_CHANNEL_2 // gpio38
#define MENU_POT_4    ADC_CHANNEL_3 // gpio39
#define LOW_PASS_POT  ADC_CHANNEL_4 // gpio32
#define SELECT_POT    ADC_CHANNEL_5 // gpio33

#define MENU_ATTEN    ADC_ATTEN_DB_12
#define MENU_UNIT     ADC_BITWIDTH_11
#define MENU_BITS     ADC_UNIT_1
#define DEFAULT_VREF  1100

#define CHOSEN_ADC_CALI_SCHEME    ESP_ADC_CAL_VAL_EFUSE_VREF

#define DEFAULT_ENVELOPE_VALS    100
#define DEFAULT_LOW_PASS_VAL     100


extern uint_fast16_t attack_val;
extern uint_fast16_t decay_val;
extern uint_fast16_t sustain_val;
extern uint_fast16_t release_val;


void task_adc();

void adc_init();

void ledc_init();

void gpio_init();


#endif // BASIC_IO_H_
