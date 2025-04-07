#ifndef SEQUENCER_H_
#define SEQUENCER_H_


#include "global_header.h"


#define	SEQ_LEN                 64
#define SEQ_VOICES              1
#define DEFAULT_SQU_TEMPO_US    1000000

#define BPM_US_FACTOR              60000000


extern int_fast8_t squ_pattern[SEQ_LEN];

extern int_fast8_t squ_index;
extern int_fast8_t squ_program_index;


void sequencer_timer_callback();
void program_sequencer(uint_fast8_t key_num);
void update_squ_timer(int_fast16_t new_val);
void sequencer_timer_init();
void pause_squ_timer();
void resume_squ_timer(int_fast16_t us_val);


#endif  // SEQUENCER_H_
