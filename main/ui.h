#ifndef UI_H_
#define UI_H_

void create_ui();
void create_menu();
void create_squ();
//void show_select_menu();
//void hide_select_menu();

void update_midi_note_name(uint_fast8_t num);
void update_ui_cb(lv_timer_t *timer);

#endif // UI_H_