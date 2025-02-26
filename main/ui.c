#include <string.h>
#include "lvgl.h"
#include "basic_io.h"
#include "data_y_splitter.h"
#include "ui.h"
    
const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 480;
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

//lv_obj_t* select_menu;

void update_ui_cb(lv_timer_t* timer) {
    //lv_obj_t* scr = lv_scr_act();
    
    //Update text values
    switch(menu_select_gui){ 
        case madsr:
            lv_label_set_text_fmt(menu_pot_1_text, "Attack: %d", attack_gui);
            lv_label_set_text_fmt(menu_pot_2_text, "Decay: %d", decay_gui);
            lv_label_set_text_fmt(menu_pot_3_text, "Sustain: %d", sustain_gui);
            lv_label_set_text_fmt(menu_pot_4_text, "Release: %d", release_gui);
            lv_bar_set_value(menu_pot_1_bar, attack_gui, DO_ANIM);
            lv_bar_set_value(menu_pot_2_bar, decay_gui, DO_ANIM);
            lv_bar_set_value(menu_pot_3_bar, sustain_gui, DO_ANIM);
            lv_bar_set_value(menu_pot_4_bar, release_gui, DO_ANIM);
            break;
        case mwave:
            lv_label_set_text_fmt(menu_pot_1_text, "Sine: %d", wave_select_gui);
            lv_label_set_text_fmt(menu_pot_2_text, "Square: %d", 0);
            lv_label_set_text_fmt(menu_pot_3_text, "Sawtooth: %d", 0);
            lv_label_set_text_fmt(menu_pot_4_text, "???: %d", 0);
            break;
        case msequencer_setup:
        case msequencer_page_1:
        case msequencer_page_2:
            lv_label_set_text_fmt(menu_pot_1_text, "PLACEHOLDER: %d", menu_select_gui);
            lv_label_set_text_fmt(menu_pot_2_text, "PLACEHOLDER: %d", 0);
            lv_label_set_text_fmt(menu_pot_3_text, "PLACEHOLDER: %d", 0);
            lv_label_set_text_fmt(menu_pot_4_text, "PLACEHOLDER: %d", 0);
            break;
    }

    
    //Update low-pass info (temporarily a bar)
    lv_label_set_text_fmt(low_pass_text, "Low-Pass:\n%d", low_pass_gui);
    lv_bar_set_value(low_pass_bar, low_pass_gui, DO_ANIM);

    //Update visualizer
    // TODO
}

void create_ui()
{
    lv_obj_t* scr = lv_scr_act();

    // Set screen background color
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    // Create visualizer panel
    lv_obj_t* viz_panel = lv_obj_create(scr);
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
    lv_obj_t* button_panel = lv_obj_create(scr);
    lv_obj_set_size(button_panel, LEFT_PANEL_WIDTH, BUTTON_HEIGHT);
    lv_obj_set_pos(scr, 0, 0);
    lv_obj_set_style_bg_color(button_panel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(button_panel, BORDER_WIDTH, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(button_panel, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(button_panel, PADDING_WIDTH, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Create 4x menu panels
    lv_obj_t* menu0 = lv_obj_create(scr);
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

    lv_obj_t* menu1 = lv_obj_create(scr);
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

    lv_obj_t* menu2 = lv_obj_create(scr);
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

    lv_obj_t* menu3 = lv_obj_create(scr);
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

    //select_menu = lv_obj_create(scr);
    // TODO

}

//void show_select_menu(){
//    lv_obj_clear_flag(select_menu, LV_OBJ_FLAG_HIDDEN);
//}

//void hide_select_menu(){
//    lv_obj_add_flag(select_menu, LV_OBJ_FLAG_HIDDEN);
//}