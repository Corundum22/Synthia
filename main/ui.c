#include <string.h>
#include "lvgl.h"
#include "basic_io.h"
#include "data_y_splitter.h"
#include "ui.h"


static lv_style_t style_screen;

lv_obj_t *menu_pot_1_text = NULL;


void update_ui_cb(lv_timer_t *timer) {
    if (lv_obj_get_screen(menu_pot_1_text) == lv_scr_act()) {
        lv_label_set_text_fmt(menu_pot_1_text, "Attack: %d", attack_gui);
    }
}


void create_ui()
{
    lv_obj_t *scr = lv_disp_get_scr_act(NULL);

    // Set the background color of the display to black.
    lv_style_init(&style_screen);
    lv_style_set_bg_color(&style_screen, lv_color_black());
    lv_obj_add_style(lv_scr_act(), &style_screen, LV_STATE_DEFAULT);

    menu_pot_1_text = lv_label_create(scr);
    lv_label_set_text(menu_pot_1_text, "Waiting");
    lv_obj_set_style_text_color(menu_pot_1_text, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(menu_pot_1_text);
    
    lv_timer_t *lv_update_timer = lv_timer_create(update_ui_cb, 120, NULL);
    lv_timer_ready(lv_update_timer);
}
