#ifndef SEQUENCER_H_
#define SEQUENCER_H_


#include "global_header.h"


#define	SEQ_LEN                 64
#define SEQ_VOICES              1
#define DEFAULT_SQU_TEMPO_US    1000000

#define TEMPO_PERIOD_MULTIPLIER    4
#define BPM_US_FACTOR              60000000


void task_sequencer();
void sequencer_timer_callback();
void update_squ_timer(int_fast16_t new_val);
void sequencer_timer_init();


#endif  // SEQUENCER_H_
