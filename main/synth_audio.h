#ifndef SYNTH_AUDIO_H_
#define SYNTH_AUDIO_H_

#define TIMER_FREQ 40000000 // 40 MHz


void task_audio_generate();
void timer_init();
void dac_init();


#endif // SYNTH_AUDIO_H_
