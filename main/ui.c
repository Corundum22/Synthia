#include <string.h>
#include "lvgl.h"
#include "basic_io.h"
#include "data_y_splitter.h"
#include "ui.h"
#include <math.h> //TODO

#define WHITE_SQUARE_BORDER lv_color_hex(0xB0B0B0)
#define BLACK_SQUARE_BORDER lv_color_hex(0x606060)


/*
TODO:   fix menu actions

ASK:    what the seq's interface is gonna look like f
*/

const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 480;
lv_obj_t* curr_scr = NULL;
lv_obj_t* scr0 = NULL;
lv_obj_t* scr1 = NULL;

//ROLLER
uint_fast8_t menu_stable = 1;

//MENU

const int BORDER_WIDTH = 5;
const int PADDING_WIDTH = 0;
const int LEFT_PANEL_WIDTH = 100;
const int VIZ_HEIGHT = 360;
const int MENU_HEIGHT = 120;
const int RIGHT_PANEL_WIDTH = SCREEN_WIDTH - LEFT_PANEL_WIDTH;
const int BUTTON_HEIGHT = SCREEN_HEIGHT - VIZ_HEIGHT;
const int MENU_BAR_HEIGHT = 30;
const int MENU_BAR_WIDTH = RIGHT_PANEL_WIDTH - 100;
const int LOW_PASS_BAR_HEIGHT = VIZ_HEIGHT - 100;
const int LOW_PASS_BAR_WIDTH = 30;
const int MENU_INNER_PADDING = 15;

lv_obj_t* menu[4];
lv_obj_t* menu_text[4];
lv_obj_t* menu_bar[4];
lv_obj_t* button_panel;
lv_obj_t* button_text;

static lv_style_t bar_bg_style;
static lv_style_t bar_ind_style;

menu_state  menu_select_gui_prev = madsr;

char* ssin_gui = "Sine";
char* striangle_gui = "Triangle";
char* ssawtooth_gui = "Sawtooth";
char* ssquare_gui = "Square";

//SQU

const int PROGRESS_BAR_HEIGHT = 10;
const int PROGRESS_BOX_HEIGHT = PROGRESS_BAR_HEIGHT + 60;
const int PROGRESS_BAR_WIDTH = 200;
const int KEY_WIDTH = 40;
const int KEY_HEIGHT = 40;
const int TOP_HEIGHT = SCREEN_HEIGHT - KEY_HEIGHT * 8;

lv_obj_t* progress_text   = NULL;
lv_obj_t* progress_bar    = NULL;
lv_obj_t* tempo_text      = NULL;
lv_obj_t* length_text     = NULL;
lv_obj_t* duration_text   = NULL;
lv_obj_t* enable_text     = NULL;

lv_obj_t* array[64][2];
char midi_note_name[] = {'a','b','c','d','\0'};

uint_fast8_t squ_enable_old = -1;

//VIZ
#define NUM_BARS 20
const int VIZ_PADDING = 2;
const int BAR_HEIGHT = (VIZ_HEIGHT - NUM_BARS*9) / NUM_BARS;

const uint_fast16_t freq_notes[] = { 8, 8, 9, 9, 10, 10, 11, 12, 12, 13, 
    14, 15, 16, 17, 18, 19, 20, 21, 23, 24, 25, 27, 29, 30, //24
    32, 34, 36, 38, 41, 43, 46, 48, 51, 55, 58, 61, 65, 69, 
    73, 77, 82, 87, 92, 97, 103, 110, 116, 123, 130, 138, 146, 
    155, 164, 174, 184, 195, 207, 220, 233, 246, 261, 277, 293, 
    311, 329, 349, 369, 391, 415, 440, 466, 493, 523, 554, 587, 
    622, 659, 698, 739, 783, 830, 880, //96
    932, 987, 1046, 1108, 
    1174, 1244, 1318, 1396, 1479, 1567, 1661, 1760, 1864, 1975, 
    2093, 2217, 2349, 2489, 2637, 2793, 2959, 3135, 3322, 3520, 
    3729, 3951, 4186, 4434, 4698, 4978, 5274, 5587, 5919, 6271, 
    6644, 7040, 7458, 7902, 8372, 8869, 9397, 9956, 10548, 11175, 
    11839, 12543};

const freq_mapping freq_map[128] = {
    { 0, 0, 255, 0 },
    { 0, 0, 255, 0 },
    { 0, 0, 255, 0 },
    { 0, 0, 255, 0 },
    { 0, 0, 255, 0 },
    { 0, 0, 255, 0 },
    { 0, 0, 255, 0 },
    { 0, 0, 255, 0 },
    { 0, 0, 255, 0 },
    { 0, 0, 255, 0 },
    { 0, 0, 255, 0 },
    { 0, 0, 255, 0 },
    { 0, 0, 255, 0 },
    { 0, 0, 255, 0 },
    { 0, 0, 255, 0 },
    { 0, 0, 255, 0 },
    { 0, 0, 255, 0 },
    { 0, 0, 255, 0 },
    { 0, 0, 255, 0 },
    { 0, 0, 255, 0 },
    { 0, 0, 255, 0 },
    { 0, 0, 255, 0 },
    { 0, 0, 255, 0 },
    { 0, 0, 255, 0 },
    { 0, 1, 183, 72 },
    { 0, 1, 100, 155 },
    { 0, 1, 13, 242 },
    { 1, 2, 187, 68 },
    { 1, 2, 102, 153 },
    { 1, 2, 13, 242 },
    { 2, 3, 192, 63 },
    { 2, 3, 114, 141 },
    { 2, 3, 31, 224 },
    { 3, 4, 204, 51 },
    { 3, 4, 121, 134 },
    { 3, 4, 33, 222 },
    { 4, 5, 204, 51 },
    { 4, 5, 122, 133 },
    { 4, 5, 34, 221 },
    { 5, 6, 208, 47 },
    { 5, 6, 130, 125 },
    { 5, 6, 46, 209 },
    { 6, 7, 218, 37 },
    { 6, 7, 136, 119 },
    { 6, 7, 48, 207 },
    { 7, 8, 219, 36 },
    { 7, 8, 140, 115 },
    { 7, 8, 55, 200 },
    { 8, 9, 227, 28 },
    { 8, 9, 147, 108 },
    { 8, 9, 63, 192 },
    { 9, 10, 234, 21 },
    { 9, 10, 155, 100 },
    { 9, 10, 72, 183 },
    { 10, 11, 241, 14 },
    { 10, 11, 161, 94 },
    { 10, 11, 76, 179 },
    { 11, 12, 243, 12 },
    { 11, 12, 164, 91 },
    { 11, 12, 80, 175 },
    { 12, 13, 247, 8 },
    { 12, 13, 170, 85 },
    { 12, 13, 87, 168 },
    { 13, 14, 255, 0 },
    { 13, 14, 176, 79 },
    { 13, 14, 93, 162 },
    { 13, 14, 5, 250 },
    { 14, 15, 181, 74 },
    { 14, 15, 99, 156 },
    { 14, 15, 11, 244 },
    { 15, 16, 187, 68 },
    { 15, 16, 105, 150 },
    { 15, 16, 18, 237 },
    { 16, 17, 192, 63 },
    { 16, 17, 110, 145 },
    { 16, 17, 22, 233 },
    { 17, 18, 197, 58 },
    { 17, 18, 116, 139 },
    { 17, 18, 30, 225 },
    { 18, 19, 203, 52 },
    { 18, 19, 122, 133 },
    { 18, 19, 35, 220 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 },
    { 19, 19, 255, 0 }
};

lv_obj_t* viz_panel       = NULL;

lv_obj_t* bars[NUM_BARS];
uint16_t bar_vals[NUM_BARS];

uint32_t mask = 0;

//ROLLER

bool menu_not_stable = false;

uint_fast16_t stable_counter = 0;
lv_obj_t* roller = NULL;


void update_ui_cb(lv_timer_t* timer) {

    
    if(menu_select_gui != menu_select_gui_prev){menu_not_stable = true;}

    if(menu_not_stable){
        lv_obj_clear_flag(roller, LV_OBJ_FLAG_HIDDEN);
        lv_roller_set_selected(roller, menu_select_gui, LV_ANIM_ON);

        if(menu_select_gui == menu_select_gui_prev){stable_counter++;}

        if(stable_counter > 10){
            menu_not_stable = false;
            stable_counter = 0;
            lv_obj_add_flag(roller, LV_OBJ_FLAG_HIDDEN);
            if(menu_select_gui == madsr || menu_select_gui == mwave){
                lv_obj_set_parent(roller, scr0);
            }
            if(menu_select_gui == msequencer_setup){
                lv_obj_set_parent(roller, scr1);
            }
        }
    }
    else {
        switch(menu_select_gui){
            case madsr:
                if(curr_scr != scr0){lv_scr_load(scr0); curr_scr = scr0;}
                if(menu_select_gui != menu_select_gui_prev){lv_obj_clear_flag(menu_bar[0], LV_OBJ_FLAG_HIDDEN); lv_obj_clear_flag(menu_bar[2], LV_OBJ_FLAG_HIDDEN); menu_select_gui_prev = madsr;}


                lv_label_set_text_fmt(menu_text[0], "Attack: %d", attack_gui);
                lv_label_set_text_fmt(menu_text[1], "Decay: %d", decay_gui);
                lv_label_set_text_fmt(menu_text[2], "Sustain: %d", sustain_gui);
                lv_label_set_text_fmt(menu_text[3], "Release: %d", release_gui);
                lv_bar_set_value(menu_bar[0], attack_gui, LV_ANIM_OFF);
                lv_bar_set_value(menu_bar[1], decay_gui, LV_ANIM_OFF);
                lv_bar_set_value(menu_bar[2], sustain_gui, LV_ANIM_OFF);
                lv_bar_set_value(menu_bar[3], release_gui, LV_ANIM_OFF);

                update_visualizer_vals();

                for(int i = 0; i < NUM_BARS; i++){
                    lv_bar_set_value(bars[i], bar_vals[i], LV_ANIM_OFF);
                }
                

                update_top_left();
                break;

            case mwave:
                if(curr_scr != scr0){lv_scr_load(scr0); curr_scr = scr0;}
                if(menu_select_gui != menu_select_gui_prev){lv_obj_add_flag(menu_bar[0], LV_OBJ_FLAG_HIDDEN); lv_obj_add_flag(menu_bar[2], LV_OBJ_FLAG_HIDDEN); menu_select_gui_prev = mwave;}

                lv_label_set_text_fmt(menu_text[0], "Wave 1: %s", get_wave_name(wave_select1_gui));
                lv_label_set_text_fmt(menu_text[1], "Blend Amount: %d", wave_blend_gui);
                lv_label_set_text_fmt(menu_text[2], "Wave 2: %s", get_wave_name(wave_select2_gui));
                lv_label_set_text_fmt(menu_text[3], "High Pass: %d", high_pass_gui);
                lv_bar_set_value(menu_bar[1], wave_blend_gui, LV_ANIM_OFF);
                lv_bar_set_value(menu_bar[3], high_pass_gui, LV_ANIM_OFF);
                        
                update_visualizer_vals();       

                for(int i = 0; i < NUM_BARS; i++){
                    lv_bar_set_value(bars[i], bar_vals[i], LV_ANIM_OFF);
                }
                
                update_top_left();
                break;

            case msequencer_setup:
                if(curr_scr != scr1){lv_scr_load(scr1); curr_scr = scr1; menu_select_gui_prev = msequencer_setup;}
                
                if(squ_enable_gui){
                    // Playback Mode

                    //Update playback pattern
                    if(squ_enable_old != squ_enable_gui){
                        for(int i = 0; i < 64; i++){
                            update_midi_note_name(squ_pattern_gui[i]);
                            lv_label_set_text(array[i][1], midi_note_name);
                            lv_obj_set_style_border_color(array[i][0], (i+i/8)%2 ? BLACK_SQUARE_BORDER : WHITE_SQUARE_BORDER, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                    uint_fast8_t prev = (squ_index_gui+63)%64;
                    lv_obj_set_style_border_color(array[prev][0], (prev+prev/8)%2 ? BLACK_SQUARE_BORDER : WHITE_SQUARE_BORDER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(array[squ_index_gui][0], lv_color_hex(0xff0000), LV_PART_MAIN | LV_STATE_DEFAULT);

                }
                else{
                    //Programming Mode
                    
                    if(squ_enable_old != squ_enable_gui){
                        for(int i = 0; i < 64; i++){
                            update_midi_note_name(squ_pattern_gui[i]);
                            lv_label_set_text(array[i][1], midi_note_name);
                            lv_obj_set_style_border_color(array[i][0], (i+i/8)%2 ? BLACK_SQUARE_BORDER : WHITE_SQUARE_BORDER, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }

                    lv_obj_set_style_border_color(array[squ_index_gui][0], (squ_index_gui+squ_index_gui/8)%2 ? BLACK_SQUARE_BORDER : WHITE_SQUARE_BORDER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    update_midi_note_name(squ_pattern_gui[squ_index_gui]);

                    if(squ_index_gui == 0){
                        for(int i = 0; i < 64; i++){
                            lv_label_set_text(array[i][1], " ");
                        }
                    }

                    lv_label_set_text_fmt(array[squ_index_gui][1], "%s", midi_note_name);
                    lv_obj_set_style_border_color(array[(squ_index_gui+1)%64][0], lv_color_hex(0x0000ff), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                
                lv_label_set_text_fmt(progress_text, "Progress: %d/%d", squ_index_gui+1, squ_length_gui);
                lv_label_set_text(enable_text, squ_enable_gui ? "Programming Mode" : "Playback Mode");
                lv_label_set_text_fmt(length_text, "Length: %d", squ_length_gui);
                lv_label_set_text_fmt(tempo_text, "Tempo: %d", squ_tempo_gui);
                lv_label_set_text_fmt(duration_text, "Duration: %d%%", squ_duration_gui);

                lv_bar_set_value(progress_bar, squ_index_gui, 1);
                squ_enable_old = squ_enable_gui;
                break;
        }
    }
    menu_select_gui_prev = menu_select_gui;
}

void create_menu() {
    curr_scr = scr0 = lv_scr_act();

    // Set screen background color
    lv_obj_set_style_bg_color(scr0, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    // Create visualizer panel
    create_visualizer();

    // Create button panel
    button_panel = lv_obj_create(scr0);
    lv_obj_set_size(button_panel, LEFT_PANEL_WIDTH, BUTTON_HEIGHT);
    lv_obj_set_pos(scr0, 0, 0);
    generic_obj_format(button_panel, lv_color_white());

    button_text = lv_label_create(button_panel);
    generic_txt_format(button_text, lv_color_black());


    // Create menu panels
    for(int i = 0; i < 4; i++){
        menu[i] = lv_obj_create(scr0);

        
        lv_obj_set_size(menu[i], RIGHT_PANEL_WIDTH, MENU_HEIGHT);
        lv_obj_set_pos(menu[i], LEFT_PANEL_WIDTH, MENU_HEIGHT*i);

        generic_obj_format(menu[i], i%2 ? lv_color_white() : lv_color_black());
        flex_column(menu[i]);

        menu_text[i] = lv_label_create(menu[i]);
        generic_txt_format(menu_text[i], i%2 ? lv_color_black() : lv_color_white());
        

        menu_bar[i] = lv_bar_create(menu[i]);

        lv_obj_remove_style_all(menu_bar[i]);

        // TODO: adjust limits
        lv_obj_set_size(menu_bar[i], MENU_BAR_WIDTH, MENU_BAR_HEIGHT);
        lv_bar_set_value(menu_bar[i], 0, LV_ANIM_OFF);
        lv_bar_set_range(menu_bar[i], 0, 255);

        bar_style(menu_bar[i], &bar_bg_style, &bar_ind_style);
    }

}

void create_squ(){
    scr1 = lv_obj_create(NULL);
    
    // Set screen background color
    lv_obj_set_style_bg_color(scr1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);

    // Create top panel
    lv_obj_t* top_panel = lv_obj_create(scr1);
    lv_obj_set_size(top_panel, SCREEN_WIDTH, 160);
    lv_obj_set_pos(top_panel, 0, 0);
    generic_obj_format(top_panel, lv_color_black());

    //progress bar
    lv_obj_t* progress_panel = lv_obj_create(top_panel);
    lv_obj_set_size(progress_panel, 300, PROGRESS_BOX_HEIGHT);
    lv_obj_set_pos(progress_panel, BORDER_WIDTH, BORDER_WIDTH);

    generic_obj_format(progress_panel, lv_color_black());
    flex_column(progress_panel);    

    progress_text = lv_label_create(progress_panel);
    lv_obj_set_style_text_color(progress_text, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_align(progress_text, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    progress_bar = lv_bar_create(progress_panel);
    lv_obj_set_size(progress_bar, PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT);
    lv_obj_set_pos(progress_panel, BORDER_WIDTH, BORDER_WIDTH);
    lv_bar_set_value(progress_bar, 0, LV_ANIM_OFF);
    lv_bar_set_range(progress_bar, 0, 64);

    // Tempo - Length - Duration - Enable
    tempo_text = lv_label_create(top_panel);
    length_text = lv_label_create(top_panel);
    duration_text = lv_label_create(top_panel);
    enable_text = lv_label_create(top_panel);

    lv_obj_set_pos(tempo_text, 20, 90);
    lv_obj_set_pos(length_text, 20, 120);
    lv_obj_set_pos(duration_text, 140, 90);
    lv_obj_set_pos(enable_text, 140, 120);
    generic_txt_format(tempo_text, lv_color_white());
    generic_txt_format(length_text, lv_color_white());
    generic_txt_format(duration_text, lv_color_white());
    generic_txt_format(enable_text, lv_color_white());
    

    //Create the 64 sequencer squares
    for(uint_fast8_t i = 0; i < 8; i++){
        for(uint_fast8_t j = 0; j < 8; j++){
            uint_fast8_t index = i*8+j;
            array[index][0] = lv_obj_create(scr1);
            //position
            lv_obj_set_size(array[index][0], KEY_WIDTH, KEY_HEIGHT);
            lv_obj_set_pos(array[index][0], j*KEY_WIDTH, TOP_HEIGHT + i*KEY_HEIGHT);
            //color
            generic_obj_format(array[index][0], (index+i)%2 ? lv_color_black() : lv_color_white());
            lv_obj_set_style_border_width(array[index][0], 2, LV_PART_MAIN | LV_STATE_DEFAULT); //TODO: fix
            //text
            array[index][1] = lv_label_create(array[index][0]);
            generic_txt_format(array[index][1], (index+i)%2 ? lv_color_white() : lv_color_black());
        }
    }
}

void update_top_left(){
    int num = 127;
    for(uint_fast8_t i = 0; i < NUM_VOICES + SEQ_VOICES; i++){
        if(note_properties_gui[i].is_sounding){
            num = note_properties_gui[i].note_num;
            break;
        }
    }
    update_midi_note_name(num);
    lv_label_set_text_fmt(button_text, "%s", midi_note_name);
}

void create_roller(){
    roller = lv_roller_create(scr0);
    lv_roller_set_options(roller, "ADSR\nWave\nSequencer", LV_ANIM_ON);
    lv_obj_add_flag(roller, LV_OBJ_FLAG_HIDDEN);
}

void update_visualizer_vals(){

    mask = 0;

    for(int i = 0; i < NUM_BARS; i++){
        bar_vals[i] = 0;
    }

    for(int i = 0; i < NUM_VOICES + SEQ_VOICES; i++){

        if(note_properties_gui[i].is_sounding){

            uint_fast8_t band1 = freq_map[note_properties_gui[i].note_num].bar1;
            mask = mask | (1 << band1);
            bar_vals[band1] += freq_map[note_properties_gui[i].note_num].weight1 * note_properties_gui[i].multiplier;

            uint_fast8_t band2 = freq_map[note_properties_gui[i].note_num].bar2;
            mask = mask | (1 << band2);
            bar_vals[band2] += freq_map[note_properties_gui[i].note_num].weight2 * note_properties_gui[i].multiplier;
        }
    }
    /*for(int i = 0; i < NUM_BARS; i++){
        if(!(mask >> i & (uint32_t)0x0001)){ 
            bar_vals[i] = 0;
        }
    }*/
}

void create_visualizer(){

    viz_panel = lv_obj_create(scr0);
    lv_obj_set_size(viz_panel, LEFT_PANEL_WIDTH, VIZ_HEIGHT);
    lv_obj_set_pos(viz_panel, 0, BUTTON_HEIGHT);

    generic_obj_format(viz_panel, lv_color_black());
    flex_column(viz_panel);

    for(int i = 0; i < NUM_BARS; i++){
        bars[i] = lv_bar_create(viz_panel);
        lv_obj_set_size(bars[i], LEFT_PANEL_WIDTH - BORDER_WIDTH*2 - VIZ_PADDING*2, BAR_HEIGHT);
        lv_obj_set_pos(bars[i], 0, VIZ_HEIGHT + BAR_HEIGHT*i);
        lv_bar_set_range(bars[i], 0, 65535);
    }
}

void generic_obj_format(lv_obj_t* o, lv_color_t c){
    lv_obj_set_style_bg_color(o, c, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(o, BORDER_WIDTH, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(o, PADDING_WIDTH, LV_PART_MAIN | LV_STATE_DEFAULT);
    if(c.full == 0){
        lv_obj_set_style_border_color(o, BLACK_SQUARE_BORDER, LV_PART_MAIN | LV_STATE_DEFAULT);
    } else if(c.full == 0xFFFF){ // RGGB
        lv_obj_set_style_border_color(o, WHITE_SQUARE_BORDER, LV_PART_MAIN | LV_STATE_DEFAULT);
    } else{
        lv_obj_set_style_border_color(o, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}

void bar_style(lv_obj_t* bar, lv_style_t* bg, lv_style_t* ind){

    lv_color_t inner_color = lv_color_hex(0x822E28);
    lv_color_t outer_color = lv_color_hex(0xCC483F); //BGR // todo
    
    //border
    lv_style_init(bg);
    lv_style_set_border_color(bg, outer_color);
    lv_style_set_border_width(bg, 4);
    lv_style_set_pad_all(bg, 7); // BW+3
    lv_style_set_radius(bg, 18);

    //inside
    lv_style_init(ind);
    lv_style_set_bg_opa(ind, LV_OPA_COVER);
    lv_style_set_bg_color(ind, inner_color);
    lv_style_set_radius(ind, 18);

    lv_obj_add_style(bar, bg, 0);
    lv_obj_add_style(bar, ind, LV_PART_INDICATOR);
}

void generic_txt_format(lv_obj_t* t, lv_color_t c){
    lv_obj_set_style_text_color(t, c, LV_PART_MAIN);
    lv_obj_set_style_text_align(t, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void flex_column(lv_obj_t* o){
    lv_obj_set_flex_flow(o, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(o, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(o, MENU_INNER_PADDING, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void create_ui(){
    create_menu();
    create_squ();
    create_visualizer();
    create_roller();
}

void update_midi_note_name(uint_fast8_t num){
    if(num > 127){
        midi_note_name[0] = '\0';
        return;
    }
    int i = 0;
    switch(num%12){
        case 0:
        case 1:
            midi_note_name[i++] = 'C';
            break;
        case 2:
            midi_note_name[i++] = 'D';
            break;
        case 3:
        case 4:
            midi_note_name[i++] = 'E';
            break;
        case 5:
        case 6:
            midi_note_name[i++] = 'F';
            break;
        case 7:
        case 8:
            midi_note_name[i++] = 'G';
            break;
        case 9:
            midi_note_name[i++] = 'A';
            break;
        case 10:
        case 11:
            midi_note_name[i++] = 'B';
            break;
        default:
    }
    switch(num%12){
        case 1:
        case 6:
        case 8:
            midi_note_name[i++] = '#';
            break;
        case 3:
        case 10:
            midi_note_name[i++] = 'b';
            break;
        default:
    }
    switch(num/12){
        case 0:
            midi_note_name[i++] = '-';
            midi_note_name[i++] = '1';
            break;
        case 1:
            midi_note_name[i++] = '0';
            break;
        case 2:
            midi_note_name[i++] = '1';
            break;
        case 3:
            midi_note_name[i++] = '2';
            break;
        case 4:
            midi_note_name[i++] = '3';
            break;
        case 5:
            midi_note_name[i++] = '4';
            break;
        case 6:
            midi_note_name[i++] = '5';
            break;
        case 7:
            midi_note_name[i++] = '6';
            break;
        case 8:
            midi_note_name[i++] = '7';
            break;
        case 9:
            midi_note_name[i++] = '8';
            break;
        case 10:
            midi_note_name[i++] = '9';
            break;
        default:
    }
    midi_note_name[i] = '\0';
}

char* get_wave_name(wave_type wave_type){
    switch(wave_type){
        case ssin:
            return ssin_gui;
        case striangle:
            return striangle_gui;
        case ssawtooth:
            return ssawtooth_gui;
        case ssquare:
            return ssquare_gui;
        default:
            return '\0';
    }
}
