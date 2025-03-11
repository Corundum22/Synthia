#ifndef UI_H_
#define UI_H_

void create_ui();
void create_menu();
void create_squ();
void create_visualizer();


void generic_obj_format(lv_obj_t* o, lv_color_t c);
void generic_txt_format(lv_obj_t* t, lv_color_t c);
void flex_column(lv_obj_t* o);
void bar_style(lv_obj_t* bar, lv_style_t* bg, lv_style_t* ind);

void update_midi_note_name(uint_fast8_t num);
void update_visualizer();
void update_top_left();
uint16_t get_freq(uint_fast8_t note_num);
uint_fast8_t get_band(uint16_t freq);

void update_ui_cb(lv_timer_t *timer);

#endif // UI_H_