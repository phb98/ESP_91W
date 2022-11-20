#include <stdio.h>
#include "std_type.h"
#include "display.h"
#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lv_conf.h"
lv_obj_t *label2, *label1;
LV_FONT_DECLARE(seg7_classic_mini_10);
LV_FONT_DECLARE(seg7_classic_mini_16);

void app_main(void)
{
  display_init();
  static lv_style_t label_style1, label_style2;
  lv_style_init(&label_style1);
  lv_style_set_text_font(&label_style1, &seg7_classic_mini_10);
  //label1 =  lv_label_create(lv_scr_act());
  //lv_obj_add_style(label1, &label_style1 ,LV_PART_MAIN | LV_STATE_DEFAULT);
  //lv_obj_set_pos(label1,40, 5);// position, position);

  lv_style_init(&label_style2);
  lv_style_set_text_font(&label_style2, &seg7_classic_mini_16);
  label2 =  lv_label_create(lv_scr_act());
  lv_obj_add_style(label2, &label_style2 ,LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_pos(label2,0, 0);// position, position);
  lv_bar_t * lv_bar = lv_bar_create(lv_scr_act());
  lv_bar_set_range(lv_bar, 0, 59);
  lv_obj_set_size(lv_bar, 64, 1);
  lv_obj_set_pos(lv_bar, 0, 18);
  uint16_t a = 1000;
  uint8_t  sec = 0;
  while(1)
  {
    char buffer[16];
    sprintf(buffer, "%02d:%02d", a/100, a%100);
    lv_label_set_text(label2, buffer);
    //lv_label_set_text(label1, "0");
    lv_bar_set_value(lv_bar,sec, LV_ANIM_OFF);
    sec++;
    if(sec > 59) {sec = 0; a++;};
    vTaskDelay(10);
  }
}
