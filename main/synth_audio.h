#ifndef SYNTH_AUDIO_H_
#define SYNTH_AUDIO_H_

#include "uart_handler.h"

#define TIMER_FREQ 40000000 // 40 MHz

#define DAC_LOWER_CHANNEL DAC_CHANNEL_MASK_CH0
#define DAC_UPPER_CHANNEL DAC_CHANNEL_MASK_CH1

#define AUDIO_BUF_SIZE        2048
#define OUTPUT_SAMPLE_RATE    96000
#define NUM_DAC_CHANNELS      1


void task_audio_generate();
void dac_init();


#endif // SYNTH_AUDIO_H_
