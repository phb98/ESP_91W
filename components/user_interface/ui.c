#include "ui.h"
#include "ui_def.h"
#include "thread_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

/*************************************************************************************/
/*                                  CONSTANT DEFINE                                  */
/*************************************************************************************/
#define UI_QUEUE_NUM            (32)
/*************************************************************************************/
/*                                  MODULE VARIABLE                                  */
/*************************************************************************************/
struct
{
  QueueHandle_t queue_handle;
  TaskHandle_t  task_handle;
  StaticTask_t  task_buffer;
  uint8_t       task_stack[CONFIG_THREAD_UI_STACK_SIZE];
} ui;
/*************************************************************************************/
/*                            PRIVATE FUNCTION PROTOTYPE                             */
/*************************************************************************************/
static void ui_thread_entry(void * param);
/*************************************************************************************/
/*                                  PUBLIC FUNCTION                                  */
/*************************************************************************************/
void ui_init()
{
  // Create Queue and Thread UI
  ui.queue_handle = xQueueCreate(UI_QUEUE_NUM, sizeof(ui_input_t));
  ui.task_handle  = xTaskCreateStatic(ui_thread_entry,
                                      CONFIG_THREAD_UI_NAME,
                                      CONFIG_THREAD_UI_STACK_SIZE,
                                      NULL,
                                      CONFIG_THREAD_UI_PRIORITY,
                                      ui.task_stack,
                                      &(ui.task_buffer));
}

/*************************************************************************************/
/*                                 PRIVATE FUNCTION                                  */
/*************************************************************************************/
static void ui_thread_entry(void * param)
{

}


