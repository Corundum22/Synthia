#ifndef SYNTH_AUDIO_H_
#define SYNTH_AUDIO_H_

#define TIMER_FREQ 40000000 // 40 MHz
#define FREQ_UP_BUTTON 12
#define FREQ_DOWN_BUTTON 14
#define GPIO_INPUT_PIN_SEL ((1ULL<<FREQ_UP_BUTTON) | (1ULL<<FREQ_DOWN_BUTTON))
#define ESP_INTR_FLAG_DEFAULT 0


uint32_t wave(uint32_t midi_note_number, uint32_t time);
void task_audio_generate();
void timer_init();
void dac_init();


#endif // SYNTH_AUDIO_H_
