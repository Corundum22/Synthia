#include <string.h>
#include "lvgl.h"
#include "basic_io.h"
#include "data_y_splitter.h"
#include "ui.h"


#define WHITE_SQUARE_BORDER lv_color_hex(0xB0B0B0)
#define BLACK_SQUARE_BORDER lv_color_hex(0x606060)

#define DEMOMODE

#ifdef DEMOMODE
#define squ_enable_gui squ_enable_gui_demo
int_fast16_t squ_enable_gui = 0;
#endif

const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 480;
lv_obj_t* curr_scr = NULL;
lv_obj_t* scr0 = NULL;
lv_obj_t* scr1 = NULL;

//MENU

const int BORDER_WIDTH = 5;
const int PADDING_WIDTH = 0;
const int LEFT_PANEL_WIDTH = 100;
const int VIZ_HEIGHT = 360;
const int MENU_HEIGHT = 120;
const int DO_ANIM = LV_ANIM_OFF;
const int RIGHT_PANEL_WIDTH = SCREEN_WIDTH - LEFT_PANEL_WIDTH;
const int BUTTON_HEIGHT = SCREEN_HEIGHT - VIZ_HEIGHT;
const int MENU_BAR_HEIGHT = 30;
const int MENU_BAR_WIDTH = RIGHT_PANEL_WIDTH - 100;
const int LOW_PASS_BAR_HEIGHT = VIZ_HEIGHT - 100;
const int LOW_PASS_BAR_WIDTH = 30;
const int MENU_INNER_PADDING = 15;
lv_obj_t* menu_pot_1_text = NULL;
lv_obj_t* menu_pot_2_text = NULL;
lv_obj_t* menu_pot_3_text = NULL;
lv_obj_t* menu_pot_4_text = NULL;
lv_obj_t* low_pass_text   = NULL;
lv_obj_t* menu_pot_1_bar  = NULL;
lv_obj_t* menu_pot_2_bar  = NULL;
lv_obj_t* menu_pot_3_bar  = NULL;
lv_obj_t* menu_pot_4_bar  = NULL;
lv_obj_t* low_pass_bar    = NULL;
lv_obj_t* tempo_text      = NULL;
lv_obj_t* length_text     = NULL;
lv_obj_t* duration_text = NULL;
lv_obj_t* enable_text     = NULL;


//SQU

const int PROGRESS_BAR_HEIGHT = 10;
const int PROGRESS_BOX_HEIGHT = PROGRESS_BAR_HEIGHT + 60;
const int PROGRESS_BAR_WIDTH = 200;
const int KEY_WIDTH = 40;
const int KEY_HEIGHT = 40;
const int TOP_HEIGHT = SCREEN_HEIGHT - KEY_HEIGHT * 8;

lv_obj_t* progress_text   = NULL;
lv_obj_t* progress_bar    = NULL;
lv_obj_t* array[64][2];
char midi_note_name[] = {'a','b','c','d','\0'};

uint_fast8_t Aindex = 0;
uint_fast8_t n = 0;
uint_fast8_t squ_enable_old = -1;

uint_fast8_t squ_test_pattern[] = { 0, 7, 14, 21, 28, 35, 
    42, 49, 56, 63, 70, 77, 84, 91, 98, 105, 112, 119, 126, 
    6, 13, 20, 27, 34, 41, 48, 55, 62, 69, 76, 83, 90, 97, 104, 
    111, 118, 125, 5, 12, 19, 26, 33, 40, 47, 54, 61, 68, 75, 
    82, 89, 96, 103, 110, 117, 124, 4, 11, 18, 25, 32, 39, 46, 
    53, 60 };

void update_ui_cb(lv_timer_t* timer) {
    
    switch(menu_select_gui){
        case madsr:
            if(curr_scr != scr0){lv_scr_load(scr0); curr_scr = scr0;}

            lv_label_set_text_fmt(menu_pot_1_text, "Attack: %d", attack_gui);
            lv_label_set_text_fmt(menu_pot_2_text, "Decay: %d", decay_gui);
            lv_label_set_text_fmt(menu_pot_3_text, "Sustain: %d", sustain_gui);
            lv_label_set_text_fmt(menu_pot_4_text, "Release: %d", release_gui);
            lv_bar_set_value(menu_pot_1_bar, attack_gui, DO_ANIM);
            lv_bar_set_value(menu_pot_2_bar, decay_gui, DO_ANIM);
            lv_bar_set_value(menu_pot_3_bar, sustain_gui, DO_ANIM);
            lv_bar_set_value(menu_pot_4_bar, release_gui, DO_ANIM);

            lv_label_set_text_fmt(low_pass_text, "Low-Pass:\n%d", low_pass_gui);
            lv_bar_set_value(low_pass_bar, low_pass_gui, DO_ANIM);
            break;

        case mwave:
            if(curr_scr != scr0){lv_scr_load(scr0); curr_scr = scr0;}

            lv_label_set_text_fmt(menu_pot_1_text, "Sine: %d", wave_select_gui);
            lv_label_set_text_fmt(menu_pot_2_text, "Square: %d", 0);
            lv_label_set_text_fmt(menu_pot_3_text, "Sawtooth: %d", 0);
            lv_label_set_text_fmt(menu_pot_4_text, "High Pass: %d", high_pass_gui);
                    
            lv_label_set_text_fmt(low_pass_text, "Low-Pass:\n%d", low_pass_gui);
            lv_bar_set_value(low_pass_bar, low_pass_gui, DO_ANIM);
            break;

        case msequencer_setup:
            if(curr_scr != scr1){lv_scr_load(scr1); curr_scr = scr1;}

            // While full sequencer functionality is unavailable, a demo of how this module will look is shown.
            // It demonstrates how programming mode will look: The red box outlines the step to be programmed
            // by pressing a key on the MIDI controller. When a note is played, the box populates with the played
            // note and advances to the next step. After 25 steps (for demo purposes) then switches to playback mode,
            // where a hypothetical fully programmed sequence is played back. In playback mode, the blue outlined
            // step is the note currently being output by the synthesizer. It advances through the sequence up to
            // step 25, then switches to programming mode.
            
            if(squ_enable_gui==0){

                // Playback Mode
                if(squ_enable_old != squ_enable_gui){
                    for(int i = 0; i < 64; i++){
                        update_midi_note_name(squ_test_pattern[i]);
                        lv_label_set_text(array[i][1], midi_note_name);
                        lv_obj_set_style_border_color(array[i][0], (i+i/8)%2 ? BLACK_SQUARE_BORDER : WHITE_SQUARE_BORDER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    }
                    Aindex = 0;
                    n = 0;
                }
                uint_fast8_t prev = (Aindex+63)%64;
                lv_obj_set_style_border_color(array[prev][0], (prev+prev/8)%2 ? BLACK_SQUARE_BORDER : WHITE_SQUARE_BORDER, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_border_color(array[Aindex][0], lv_color_hex(0xff0000), LV_PART_MAIN | LV_STATE_DEFAULT);

                #ifdef DEMOMODE
                if(n == 5){
                    Aindex = (Aindex+1)%64;
                    n = 0;
                }
                else{n++;}
                #endif
            }
            else{

                //Programming Mode
                if(squ_enable_old != squ_enable_gui){
                    for(int i = 0; i < 64; i++){
                        update_midi_note_name(squ_test_pattern[i]);
                        lv_label_set_text(array[i][1], midi_note_name);
                        lv_obj_set_style_border_color(array[i][0], (i+i/8)%2 ? BLACK_SQUARE_BORDER : WHITE_SQUARE_BORDER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    }
                    Aindex = 0;
                    n = 0;
                }

                lv_obj_set_style_border_color(array[Aindex][0], (Aindex+Aindex/8)%2 ? BLACK_SQUARE_BORDER : WHITE_SQUARE_BORDER, LV_PART_MAIN | LV_STATE_DEFAULT);
                update_midi_note_name(squ_test_pattern[Aindex]);

                if(Aindex == 0){
                    for(int i = 0; i < 64; i++){
                        lv_label_set_text(array[i][1], " ");
                    }
                }

                lv_label_set_text_fmt(array[Aindex][1], "%s", midi_note_name);
                lv_obj_set_style_border_color(array[(Aindex+1)%64][0], lv_color_hex(0x0000ff), LV_PART_MAIN | LV_STATE_DEFAULT);

                #ifdef DEMOMODE
                if(n == 5){
                    Aindex = (Aindex+1)%64;
                    n = 0;
                }
                else{n++;}
                #endif
            }
            
            lv_label_set_text_fmt(progress_text, "Progress: %d/%d", Aindex+1, squ_length_gui);
            lv_label_set_text(enable_text, squ_enable_gui ? "Playback Mode" : "Programming Mode");
            lv_label_set_text_fmt(length_text, "Length: %d", squ_length_gui);
            lv_label_set_text_fmt(tempo_text, "Tempo: %d", squ_tempo_gui);
            lv_label_set_text_fmt(duration_text, "Duration: %d%%", squ_duration_gui);

            lv_bar_set_value(progress_bar, Aindex, 1);
            squ_enable_old = squ_enable_gui;

            if(Aindex==25){
                squ_enable_gui = squ_enable_gui ? 0 : 1; 
            }
            break;

    }
}

void create_menu()
{
    curr_scr = scr0 = lv_scr_act();

    // Set screen background color
    lv_obj_set_style_bg_color(scr0, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    // Create visualizer panel
    lv_obj_t* viz_panel = lv_obj_create(scr0);
    lv_obj_set_size(viz_panel, LEFT_PANEL_WIDTH, VIZ_HEIGHT);
    lv_obj_set_pos(viz_panel, 0, BUTTON_HEIGHT);
    lv_obj_set_style_bg_color(viz_panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(viz_panel, BORDER_WIDTH, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(viz_panel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(viz_panel, PADDING_WIDTH, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_flex_flow(viz_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(viz_panel, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(viz_panel, MENU_INNER_PADDING, LV_PART_MAIN | LV_STATE_DEFAULT);

    low_pass_text = lv_label_create(viz_panel);
    lv_obj_set_style_text_color(low_pass_text, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_align(low_pass_text, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    low_pass_bar = lv_bar_create(viz_panel);
    lv_obj_set_size(low_pass_bar, LOW_PASS_BAR_WIDTH, LOW_PASS_BAR_HEIGHT);
    lv_bar_set_value(low_pass_bar, 0, DO_ANIM);


    // Create button panel
    lv_obj_t* button_panel = lv_obj_create(scr0);
    lv_obj_set_size(button_panel, LEFT_PANEL_WIDTH, BUTTON_HEIGHT);
    lv_obj_set_pos(scr0, 0, 0);
    lv_obj_set_style_bg_color(button_panel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(button_panel, BORDER_WIDTH, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(button_panel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(button_panel, PADDING_WIDTH, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Create 4x menu panels
    lv_obj_t* menu0 = lv_obj_create(scr0);
    lv_obj_set_size(menu0, RIGHT_PANEL_WIDTH, MENU_HEIGHT);
    lv_obj_set_pos(menu0, LEFT_PANEL_WIDTH, 0);
    lv_obj_set_style_bg_color(menu0, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(menu0, BORDER_WIDTH, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(menu0, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(menu0, PADDING_WIDTH, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_flex_flow(menu0, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(menu0, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(menu0, MENU_INNER_PADDING, LV_PART_MAIN | LV_STATE_DEFAULT);

    menu_pot_1_text = lv_label_create(menu0);
    lv_obj_set_style_text_color(menu_pot_1_text, lv_color_white(), LV_PART_MAIN);

    menu_pot_1_bar = lv_bar_create(menu0);
    lv_obj_set_size(menu_pot_1_bar, MENU_BAR_WIDTH, MENU_BAR_HEIGHT);
    lv_bar_set_value(menu_pot_1_bar, 0, DO_ANIM);

    lv_obj_t* menu1 = lv_obj_create(scr0);
    lv_obj_set_size(menu1, RIGHT_PANEL_WIDTH, MENU_HEIGHT);
    lv_obj_set_pos(menu1, LEFT_PANEL_WIDTH, MENU_HEIGHT);
    lv_obj_set_style_bg_color(menu1, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(menu1, BORDER_WIDTH, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(menu1, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(menu1, PADDING_WIDTH, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_flex_flow(menu1, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(menu1, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(menu1, MENU_INNER_PADDING, LV_PART_MAIN | LV_STATE_DEFAULT);

    menu_pot_2_text = lv_label_create(menu1);
    lv_obj_set_style_text_color(menu_pot_2_text, lv_color_black(), LV_PART_MAIN);

    menu_pot_2_bar = lv_bar_create(menu1);
    lv_obj_set_size(menu_pot_2_bar, MENU_BAR_WIDTH, MENU_BAR_HEIGHT);
    lv_bar_set_value(menu_pot_2_bar, 0, DO_ANIM);

    lv_obj_t* menu2 = lv_obj_create(scr0);
    lv_obj_set_size(menu2, RIGHT_PANEL_WIDTH, MENU_HEIGHT);
    lv_obj_set_pos(menu2, LEFT_PANEL_WIDTH, MENU_HEIGHT*2);
    lv_obj_set_style_bg_color(menu2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(menu2, BORDER_WIDTH, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(menu2, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(menu2, PADDING_WIDTH, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_flex_flow(menu2, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(menu2, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(menu2, MENU_INNER_PADDING, LV_PART_MAIN | LV_STATE_DEFAULT);

    menu_pot_3_text = lv_label_create(menu2);
    lv_obj_set_style_text_color(menu_pot_3_text, lv_color_white(), LV_PART_MAIN);

    menu_pot_3_bar = lv_bar_create(menu2);
    lv_obj_set_size(menu_pot_3_bar, MENU_BAR_WIDTH, MENU_BAR_HEIGHT);
    lv_bar_set_value(menu_pot_3_bar, 0, DO_ANIM);

    lv_obj_t* menu3 = lv_obj_create(scr0);
    lv_obj_set_size(menu3, RIGHT_PANEL_WIDTH, MENU_HEIGHT);
    lv_obj_set_pos(menu3, LEFT_PANEL_WIDTH, MENU_HEIGHT*3);
    lv_obj_set_style_bg_color(menu3, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(menu3, BORDER_WIDTH, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(menu3, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(menu3, PADDING_WIDTH, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_flex_flow(menu3, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(menu3, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(menu3, MENU_INNER_PADDING, LV_PART_MAIN | LV_STATE_DEFAULT);

    menu_pot_4_text = lv_label_create(menu3);
    lv_obj_set_style_text_color(menu_pot_4_text, lv_color_black(), LV_PART_MAIN);

    menu_pot_4_bar = lv_bar_create(menu3);
    lv_obj_set_size(menu_pot_4_bar, MENU_BAR_WIDTH, MENU_BAR_HEIGHT);
    lv_bar_set_value(menu_pot_4_bar, 0, DO_ANIM);
}

void create_squ(){
    scr1 = lv_obj_create(NULL);
    
    // Set screen background color
    lv_obj_set_style_bg_color(scr1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);

    // Create top panel
    lv_obj_t* top_panel = lv_obj_create(scr1);
    lv_obj_set_size(top_panel, SCREEN_WIDTH, 160);
    lv_obj_set_pos(top_panel, 0, 0);
    lv_obj_set_style_bg_color(top_panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(top_panel, BORDER_WIDTH, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(top_panel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(top_panel, PADDING_WIDTH, LV_PART_MAIN | LV_STATE_DEFAULT);

    //progress bar
    lv_obj_t* progress_panel = lv_obj_create(top_panel);
    lv_obj_set_size(progress_panel, 300, PROGRESS_BOX_HEIGHT);
    lv_obj_set_pos(progress_panel, BORDER_WIDTH, BORDER_WIDTH);
    lv_obj_set_style_bg_color(progress_panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(progress_panel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(progress_panel, BORDER_WIDTH, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_flex_flow(progress_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(progress_panel, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(progress_panel, MENU_INNER_PADDING, LV_PART_MAIN | LV_STATE_DEFAULT);


    progress_text = lv_label_create(progress_panel);
    lv_obj_set_style_text_color(progress_text, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_align(progress_text, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    progress_bar = lv_bar_create(progress_panel);
    lv_obj_set_size(progress_bar, PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT);
    lv_obj_set_pos(progress_panel, BORDER_WIDTH, BORDER_WIDTH);
    lv_bar_set_value(progress_bar, 0, DO_ANIM);
    lv_bar_set_range(progress_bar, 0, 64);

    // Tempo - Length - Duration - Enable
    tempo_text = lv_label_create(top_panel);
    length_text = lv_label_create(top_panel);
    duration_text = lv_label_create(top_panel);
    enable_text = lv_label_create(top_panel);

    lv_obj_set_pos(tempo_text, 20, 90);
    lv_obj_set_style_text_color(tempo_text, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_align(tempo_text, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_pos(length_text, 20, 120);
    lv_obj_set_style_text_color(length_text, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_align(length_text, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_pos(duration_text, 140, 90);
    lv_obj_set_style_text_color(duration_text, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_align(duration_text, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_pos(enable_text, 140, 120);
    lv_obj_set_style_text_color(enable_text, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_align(enable_text, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);


    //Create the 64 sequencer squares
    for(uint_fast8_t i = 0; i < 8; i++){
        for(uint_fast8_t j = 0; j < 8; j++){
            uint_fast8_t index = i*8+j;
            //square
            array[index][0] = lv_obj_create(scr1);
            lv_obj_set_size(array[index][0], KEY_WIDTH, KEY_HEIGHT);
            lv_obj_set_pos(array[index][0], j*KEY_WIDTH, TOP_HEIGHT + i*KEY_HEIGHT);
            lv_obj_set_style_bg_color    (array[index][0], (index+i)%2 ? lv_color_black()    : lv_color_white(),    LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(array[index][0], (index+i)%2 ? BLACK_SQUARE_BORDER : WHITE_SQUARE_BORDER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_all(array[index][0], 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            //text
            array[index][1] = lv_label_create(array[index][0]);
            lv_obj_set_style_text_color(array[index][1], (index+i)%2 ? lv_color_white() : lv_color_black(), LV_PART_MAIN);
            lv_obj_set_style_text_align(array[index][1], LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
}

void create_ui(){
    create_menu();
    create_squ();
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
