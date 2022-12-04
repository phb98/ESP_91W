#include "home_screen.h"
#include "ui.h"
#include "lvgl.h"
#include "lvgl_font.h"
#include "esp_log.h"

/*************************************************************************************/
/*                                  CONSTANT DEFINE                                  */
/*************************************************************************************/
#define HS_LOGI(...) ESP_LOGI("Home Screen", __VA_ARGS__)   
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
} home_screen;

/*************************************************************************************/
/*                            PRIVATE FUNCTION PROTOTYPE                             */
/*************************************************************************************/
static void home_screen_handler(ui_input_t * input, ui_output_t * output);
static void home_screen_draw(lv_obj_t *p_parent);
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
      break;
    default:
      break;
  }
}

/**
 * draw the home screen
*/
static void home_screen_draw(lv_obj_t *p_parent)
{
  lv_obj_t * clock_num;
  char time_string[16];
  lv_snprintf_builtin(time_string, sizeof(time_string), "%02d:%02d", home_screen.current_time.hour, home_screen.current_time.min);
  clock_num = lv_label_create(p_parent);
  //lv_obj_set_pos(clock_num, 0, 10);
  lv_label_set_text(clock_num, time_string);
  lv_obj_center(clock_num);
}