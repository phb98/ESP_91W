#include "ui_timer.h"
#include "ui.h"
#include "ui_helper.h"
#include "esp_log.h"
#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
/*************************************************************************************/
/*                                  CONSTANT DEFINE                                  */
/*************************************************************************************/
#define MAX_UI_TIMER            (16)
#define TIMER_NAME_FORMAT       "%s_%s"
#define TIMER_INVALID_INTERVAL  (0xFFFFFFFF)
#define UI_TIMER_LOGI(...)  ESP_LOGI("UI_TIMER", __VA_ARGS__)
/*************************************************************************************/
/*                                    MODULE TYPE                                    */
/*************************************************************************************/
typedef enum
{
  UI_TIMER_IDLE,
  UI_TIMER_RUNNING
} ui_timer_state_t;
typedef struct
{
  uint64_t start_time_ms;
  uint32_t timeout_ms;
  uint32_t crc;
  ui_timer_state_t state;
} ui_timer_t;

/*************************************************************************************/
/*                                  MODULE VARIABLE                                  */
/*************************************************************************************/
static struct
{
  ui_timer_t    timer_list[MAX_UI_TIMER];
  StaticTimer_t timer_buffer;
  TimerHandle_t timer_handle;
} ui_timer;
/*************************************************************************************/
/*                            PRIVATE FUNCTION PROTOTYPE                             */
/*************************************************************************************/
static void ui_timer_stop_schedule();
static void ui_timer_start_schedule();
static ui_timer_t * ui_timer_get_timer_from_crc(uint32_t crc);
static void ui_timer_expired(TimerHandle_t xTimer);
/*************************************************************************************/
/*                                  PUBLIC FUNCTION                                  */
/*************************************************************************************/
void ui_timer_init()
{
  ui_timer.timer_handle = xTimerCreateStatic("UI TIMER", 100, pdFALSE, (void*)0, ui_timer_expired, &ui_timer.timer_buffer);
}

void ui_timer_start(ui_node_t * p_node, char * timer_name, uint32_t timeout)
{
  if(!p_node || !timer_name) return;

  //temporary stop timer to add and reschedule 
  ui_timer_stop_schedule();
  // Create timer name by combine node name and timer name
  char combine_name[64];
  memset(combine_name, 0x0, sizeof(combine_name));
  snprintf(combine_name, sizeof(combine_name), TIMER_NAME_FORMAT, p_node->node_info.node_name, timer_name);

  uint32_t timer_crc = ui_calc_crc32((uint8_t*)combine_name, strlen(combine_name));
  UI_TIMER_LOGI("Add timer %lu", timer_crc);
  ui_timer_t * p_timer = ui_timer_get_timer_from_crc(timer_crc);
  
  //setup timer parameter
  uint64_t current_ms = xTaskGetTickCount() * (portTICK_PERIOD_MS);
  p_timer->start_time_ms = current_ms;
  p_timer->timeout_ms    = timeout;
  p_timer->state         = UI_TIMER_RUNNING;
  p_timer->crc           = timer_crc;
  // restart timer schedule
  ui_timer_start_schedule();
}

bool ui_timer_is_expired(ui_node_t * p_node, char * timer_name, uint32_t timer_crc)
{
  char combine_name[64];
  memset(combine_name, 0x0, sizeof(combine_name));
  snprintf(combine_name, sizeof(combine_name), TIMER_NAME_FORMAT, p_node->node_info.node_name, timer_name);
  uint32_t crc = ui_calc_crc32((uint8_t*)combine_name, strlen(combine_name));
  UI_TIMER_LOGI("check timer %lu and %lu", timer_crc, crc);
  return timer_crc == crc;
}
/*************************************************************************************/
/*                                 PRIVATE FUNCTION                                  */
/*************************************************************************************/

static void ui_timer_stop_schedule()
{
  xTimerStop(ui_timer.timer_handle, portMAX_DELAY);
}

static ui_timer_t * ui_timer_get_timer_from_crc(uint32_t crc)
{
  // check if this timer exist
  ui_timer_t * free_timer = NULL;
  for(uint32_t i = 0; i < MAX_UI_TIMER; i++)
  {
    ui_timer_t * p_timer = &(ui_timer.timer_list[i]);
    if(p_timer->crc == crc && p_timer->state == UI_TIMER_RUNNING)
    {
      UI_TIMER_LOGI("Timer already in list");
      return p_timer;
    }
    else if (p_timer->state == UI_TIMER_IDLE)
    {
      free_timer = p_timer;
    }
  }
  // this timer does not exist, allocate one
  UI_TIMER_LOGI("Timer not in list");
  assert(free_timer != NULL);
  return free_timer;
}

static void ui_timer_start_schedule()
{
  uint64_t current_ms = xTaskGetTickCount() * (portTICK_PERIOD_MS);
  // List through time list to find which timer is expired and calculate how much time to schedule time
  uint32_t min_schedule_time_ms = TIMER_INVALID_INTERVAL;
  for(int i = 0; i < MAX_UI_TIMER; i++)
  {
    ui_timer_t * p_timer = &(ui_timer.timer_list[i]);
    if(p_timer->state == UI_TIMER_RUNNING)
    {
      if(((current_ms - p_timer->start_time_ms) >= (p_timer->timeout_ms)))
      {
        //Send input to ui
        ui_input_t timer_expired_evt = 
        {
          .evt = UI_EVT_TIMER_EXPIRED,
          .param.timer_expired.crc = p_timer->crc,
          .param_length = sizeof(uint32_t)
        };
        UI_TIMER_LOGI("Timer:%lu Finished", p_timer->crc);
        memset(p_timer, 0x0, sizeof(ui_timer_t));
        p_timer->state = UI_TIMER_IDLE;
        ui_input_handle(&timer_expired_evt);
      }
      else
      {
        uint64_t remain_time = p_timer->timeout_ms - (current_ms - p_timer->start_time_ms);
        if(remain_time < min_schedule_time_ms)  min_schedule_time_ms = remain_time;
      }
    }
  }
  if(min_schedule_time_ms != TIMER_INVALID_INTERVAL)
  {
    xTimerChangePeriod(ui_timer.timer_handle, min_schedule_time_ms / portTICK_PERIOD_MS, portMAX_DELAY);
    // Start os timer 
    UI_TIMER_LOGI("Start OS timer:%d", (int)min_schedule_time_ms);
  }
  else UI_TIMER_LOGI("Not restart OS timer");
}

static void ui_timer_expired(TimerHandle_t xTimer )
{
  uint64_t current_ms = xTaskGetTickCount() * (portTICK_PERIOD_MS);
  // List through time list to find which timer is expired and calculate how much time to schedule time
  for(int i = 0; i < MAX_UI_TIMER; i++)
  {
    ui_timer_t * p_timer = &(ui_timer.timer_list[i]);
    if(p_timer->state == UI_TIMER_RUNNING)
    {
      if(((current_ms - p_timer->start_time_ms) >= (p_timer->timeout_ms)))
      {
        //Send input to ui
        ui_input_t timer_expired_evt = 
        {
          .evt = UI_EVT_TIMER_EXPIRED,
          .param.timer_expired.crc = p_timer->crc,
          .param_length = sizeof(uint32_t)
        };
        UI_TIMER_LOGI("Timer:%lu Finished", p_timer->crc);
        memset(p_timer, 0x0, sizeof(ui_timer_t));
        p_timer->state = UI_TIMER_IDLE;
        ui_input_handle(&timer_expired_evt);
      }
    }
  }
  UI_TIMER_LOGI("Timer expired");
}

