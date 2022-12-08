#ifndef _UI_DEF_H
#define _UI_DEF_H
#include <stdint.h>


typedef enum
{
  UI_EVT_SYS_INIT = 1,
  UI_EVT_TIME_UPDATE = 2,
  UI_EVT_TIMER_EXPIRED = 3,
  NUM_UI_EVT,
} ui_input_evt_t;

typedef struct
{
  uint8_t hour;
  uint8_t min;
  uint8_t sec;
  uint16_t year;
  uint8_t month;
  uint8_t day;
} ui_time_update_t;

typedef struct
{
  uint32_t crc;
} ui_timer_expired_t;

typedef struct
{
  ui_input_evt_t evt;
  union
  {
    uint8_t raw[1];
    ui_time_update_t time_update;
    ui_timer_expired_t timer_expired;
  } param;
  uint32_t param_length;
} ui_input_t;

typedef struct
{
  uint8_t    request_draw;
  ui_input_t *re_input;

} ui_output_t;

typedef enum
{
  UI_RET_OK,
  UI_RET_INVALID_PARAM,
} ui_ret_t;

#endif