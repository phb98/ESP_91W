#include "ui.h"
#include "ui_def.h"
#include "ui_timer.h"
#include "thread_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include <string.h>
#include "lvgl.h"

#include "home_screen.h"
/*************************************************************************************/
/*                                  CONSTANT DEFINE                                  */
/*************************************************************************************/
#define UI_QUEUE_NUM            (32)
#define UI_LOGI(...)            ESP_LOGI("UI", __VA_ARGS__)
/*************************************************************************************/
/*                                  MODULE VARIABLE                                  */
/*************************************************************************************/
struct
{
  QueueHandle_t input_queue_handle;
  TaskHandle_t  task_handle;
  StaticTask_t  task_buffer;
  uint8_t       task_stack[CONFIG_THREAD_UI_STACK_SIZE];
  ui_node_t     *child_node[MAX_CHILD_NODE];
  uint8_t       num_child_node;
} ui;

/*************************************************************************************/
/*                            PRIVATE FUNCTION PROTOTYPE                             */
/*************************************************************************************/
static void ui_thread_entry(void * param);
static void ui_node_process_input(ui_node_t * p_child_node, ui_input_t * input, ui_output_t * output);
/*************************************************************************************/
/*                                  PUBLIC FUNCTION                                  */
/*************************************************************************************/
void ui_init()
{
  UI_LOGI("UI INIT");
  ui_timer_init();
  // Init child node
  home_screen_init();
  // Create Queue and Thread UI
  ui.input_queue_handle = xQueueCreate(UI_QUEUE_NUM, sizeof(ui_input_t));
  ui.task_handle  = xTaskCreateStatic(ui_thread_entry,
                                      CONFIG_THREAD_UI_NAME,
                                      CONFIG_THREAD_UI_STACK_SIZE,
                                      NULL,
                                      CONFIG_THREAD_UI_PRIORITY,
                                      ui.task_stack,
                                      &(ui.task_buffer));
  // Post the first input
  ui_input_t sys_init_input = 
  {
    .evt = UI_EVT_SYS_INIT,
  };
  ui_input_handle(&sys_init_input);

}

ui_ret_t ui_input_handle(ui_input_t * ui_input)
{
  if(!ui_input || (ui_input->evt >= NUM_UI_EVT)) return UI_RET_INVALID_PARAM;
  UI_LOGI("UI INPUT EVT:%d", ui_input->evt);

  // Push to queue to handle in ui thread
  xQueueSend(ui.input_queue_handle, ui_input, portMAX_DELAY);
  return UI_RET_OK;
}

void * ui_malloc(uint32_t size)
{
  return malloc(size);
}

ui_ret_t ui_node_register(ui_node_t * p_node)
{
  if(!p_node) return UI_RET_INVALID_PARAM;
  // memcpy(&ui.child_node[ui.num_child_node], p_node, sizeof(ui_node_t));
  ui.child_node[ui.num_child_node] = p_node;
  ui.num_child_node++;
  return UI_RET_OK;
}
/*************************************************************************************/
/*                                 PRIVATE FUNCTION                                  */
/*************************************************************************************/
static void ui_thread_entry(void * param)
{
  while(1)
  {
    // wait for input event
    ui_input_t input_evt;
    xQueueReceive(ui.input_queue_handle, &input_evt, portMAX_DELAY);
    UI_LOGI("Receive event:%d", input_evt.evt);
    // Pass the input to all child node
    ui_output_t node_output;
    lv_obj_t * p_parent = lv_scr_act();

    for(int i = 0; i < ui.num_child_node; i++)
    {
      ui_node_t * p_child_node = ui.child_node[i];
      ui_node_process_input(p_child_node, &input_evt, &node_output);
      if(node_output.request_draw)
      {
        p_child_node->draw(p_parent);
      }
    }
  }
}

static void ui_node_process_input(ui_node_t * p_child_node, ui_input_t * input, ui_output_t * output)
{
  if(!input || !output || !p_child_node) return;
  if(p_child_node->run)
  {
    p_child_node->run(input, output);
    if(output->request_draw && p_child_node->draw)
    {
      p_child_node->draw(lv_scr_act());
    }
  }
}
