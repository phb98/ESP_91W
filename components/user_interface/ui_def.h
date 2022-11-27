#ifndef _UI_DEF_H
#define _UI_DEF_H
#include <stdint.h>
typedef enum
{
  UI_EVT_INIT = 1,
  UI_EVT_TIME_UPDATE = 2,
} ui_event_type_t;

typedef enum
{
  NODE_CLASS_HOME,
  NODE_CLASS_APP,
} ui_node_class_t;
typedef struct
{
  ui_event_type_t evt;
  union{
    struct
    {
      
    } raw;
  } param;
  uint32_t param_length;
} ui_input_t;

typedef struct
{
  uint8_t    request_draw;
  ui_input_t re_input;

} ui_output_t;
typedef struct
{
  struct {
    char            *node_name;
    ui_node_class_t node_class;
  } node_info;
  void (*run)(ui_input_t * input, ui_output_t * output);
  void (*draw);
} ui_node_t;
#endif