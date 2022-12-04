#ifndef _UI_H
#define _UI_H
#include "ui_def.h"
#include "lvgl.h"
/*************************************************************************************/
/*                                  CONSTANT DEFINE                                  */
/*************************************************************************************/
#define MAX_CHILD_NODE  (10)
/*************************************************************************************/
/*                                    MODULE TYPE                                    */
/*************************************************************************************/
typedef enum
{
  NODE_CLASS_HOME,
  NODE_CLASS_APP,
} ui_node_class_t;

typedef struct
{
  struct 
  {
    char            *node_name;
    ui_node_class_t node_class;
  } node_info;
  void (*run)(ui_input_t * input, ui_output_t * output);
  void (*draw)(lv_obj_t * p_parent);
} ui_node_t;

/*************************************************************************************/
/*                                  PUBLIC FUNCTION                                  */
/*************************************************************************************/
void     ui_init();
void *   ui_malloc(uint32_t size);
ui_ret_t ui_node_register(ui_node_t * p_node);
ui_ret_t ui_input_handle(ui_input_t * input);
#endif
