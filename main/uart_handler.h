#ifndef UART_HANDLER_H_
#define UART_HANDLER_H_


#define MIDI_UART_BAUD_RATE     31250
#define MIDI_UART_DATA_BITS     UART_DATA_8_BITS
#define MIDI_UART_PARITY        UART_PARITY_DISABLE
#define MIDI_UART_STOP_BITS     UART_STOP_BITS_1
#define MIDI_UART_BUFFER_SIZE   1024
#define MIDI_UART_NUM           UART_NUM_2

#define NOTE_ON                 (0b001 << 4)
#define NOTE_OFF                (0b000 << 4)
#define STATUS_BIT              0b10000000

#define NUM_VOICES              8


extern uint_fast8_t on_notes[8];


void uart_init();

void task_midi_uart(void *pvParameters);


#endif // UART_HANDLER_H_
