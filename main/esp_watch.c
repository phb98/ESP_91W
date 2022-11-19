#include <stdio.h>
#include "std_type.h"
#include "display.h"
#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
lv_obj_t *label2, *label1;
void app_main(void)
{
  display_init();
  lv_obj_t * scr = lv_disp_get_scr_act(NULL);
  label1 =  lv_label_create(lv_scr_act());
  lv_label_set_text(label1, LV_SYMBOL_WIFI);
  lv_obj_set_pos(label1,0, 0);// position, position);
  lv_obj_t * bar1 = lv_bar_create(lv_scr_act());
  lv_obj_set_size(bar1, 30, 5);
  lv_obj_center(bar1);
  lv_bar_set_value(bar1, 70, LV_ANIM_OFF);
}
