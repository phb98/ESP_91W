#include "home_screen.h"
#include "ui.h"
#include "lvgl.h"
#include "lvgl_font.h"
#include "esp_log.h"
#include "ui_timer.h"
/*************************************************************************************/
/*                                  CONSTANT DEFINE                                  */
/*************************************************************************************/
#define HS_LOGI(...)          ESP_LOGI("Home Screen", __VA_ARGS__)   
#define UPDATE_TIME_INTERVAL  (60*60*1000) // 1 HOUR
/*************************************************************************************/
/*                                    MODULE TYPE                                    */
/*************************************************************************************/
static ui_node_t node_home_screen =
{
  .node_info.node_name = "HOME_SCREEN",
  .node_info.node_class = NODE_CLASS_HOME,
};
/*************************************************************************************/
/*                                  MODULE VARIABLE                                  */
/*************************************************************************************/
static struct
{
  struct
  {
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint16_t year;
    uint8_t month;
    uint8_t day;    
  } current_time;
  struct
  {
    uint8_t is_init;
    lv_obj_t * clock_label;
    lv_style_t clock_label_style;
  } ui_obj;
} home_screen;

/*************************************************************************************/
/*                            PRIVATE FUNCTION PROTOTYPE                             */
/*************************************************************************************/
static void home_screen_handler(ui_input_t * input, ui_output_t * output);
static void home_screen_draw(lv_obj_t *p_parent);
static void home_screen_lvgl_obj_init(lv_obj_t * p_parent);
/*************************************************************************************/
/*                                  PUBLIC FUNCTION                                  */
/*************************************************************************************/
void home_screen_init()
{
  node_home_screen.run = home_screen_handler;
  node_home_screen.draw = home_screen_draw;
  ui_node_register(&node_home_screen); 
}
/*************************************************************************************/
/*                                 PRIVATE FUNCTION                                  */
/*************************************************************************************/

static void home_screen_handler(ui_input_t * input, ui_output_t * output)
{
  assert(input && output);
  HS_LOGI("Evt:%d", input->evt);
  switch(input->evt)
  {
    case UI_EVT_SYS_INIT:
      output->request_draw = true;
      break;
    case UI_EVT_TIME_UPDATE:
      memcpy(&home_screen.current_time, input->param.raw, sizeof(ui_time_update_t)); // hack , only for testing
      output->request_draw = true;
      ui_timer_start(&node_home_screen, "TIME_UPDATE", 1000);
      break;
    case UI_EVT_TIMER_EXPIRED:
      if(ui_timer_is_expired(&node_home_screen,"TIME_UPDATE", input->param.timer_expired.crc))
      {
        HS_LOGI("TIMER EXPIRED");
        ui_timer_start(&node_home_screen, "TIME_UPDATE", 1000);
      }
    default:
      break;
  }
}

/**
 * draw the home screen
*/
static void home_screen_draw(lv_obj_t *p_parent)
{
  home_screen_lvgl_obj_init(p_parent);
  lv_style_set_text_font(&home_screen.ui_obj.clock_label_style, &seven_seg_20);
  lv_label_set_text_fmt(home_screen.ui_obj.clock_label, "%02d %02d", home_screen.current_time.hour, home_screen.current_time.min);
}

static void home_screen_lvgl_obj_init(lv_obj_t * p_parent)
{
  if(home_screen.ui_obj.is_init) return;
  // Create label clock Number
  home_screen.ui_obj.clock_label = lv_label_create(p_parent);
  // Cteate style for label clock number
  lv_style_init(&home_screen.ui_obj.clock_label_style);
  lv_obj_add_style(home_screen.ui_obj.clock_label, &home_screen.ui_obj.clock_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);

  home_screen.ui_obj.is_init = true;
}