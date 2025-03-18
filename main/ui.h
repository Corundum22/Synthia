#ifndef UI_H_
#define UI_H_

typedef struct {
    uint_fast8_t bar1;
    uint_fast8_t bar2;
    uint_fast16_t weight1;
    uint_fast16_t weight2;
} freq_mapping;

void create_ui();
void create_menu();
void create_squ();
void create_visualizer();
void create_roller();


void generic_obj_format(lv_obj_t* o, lv_color_t c);
void generic_txt_format(lv_obj_t* t, lv_color_t c);
void flex_column(lv_obj_t* o);
void bar_style(lv_obj_t* bar, lv_style_t* bg, lv_style_t* ind);
char* get_wave_name(wave_type wave_type);

void update_midi_note_name(uint_fast8_t num);
void update_visualizer_vals();
void update_top_left();
uint_fast8_t get_band(uint_fast8_t num);

void update_ui_cb(lv_timer_t *timer);

#endif // UI_H_
