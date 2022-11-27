#ifndef _THREAD_CONFIG_H
#define _THREAD_CONFIG_H
/*************************************************************************************/
/*                                  CONSTANT DEFINE                                  */
/*************************************************************************************/
#define THREAD_IDLE_PRIORITY                      (0)
// Thread Display
#define CONFIG_THREAD_DISPLAY_NAME                ("DISPLAY_THREAD")
#define CONFIG_THREAD_DISPLAY_STACK_SIZE          (8*1024)
#define CONFIG_THREAD_DISPLAY_PRIORITY            (THREAD_IDLE_PRIORITY + 3)
// Thread BLE_SVC_DIS
#define CONFIG_THREAD_BLE_SVC_DIS_NAME            ("BLE_SVC_DIS_THREAD")
#define CONFIG_THREAD_BLE_SVC_DIS_STACK_SIZE      (4*1024)
#define CONFIG_THREAD_BLE_SVC_DIS_PRIORITY        (THREAD_IDLE_PRIORITY + 2)
// Thread UI
#define CONFIG_THREAD_UI_NAME                     ("UI_THREAD")
#define CONFIG_THREAD_UI_STACK_SIZE               (4*1024)
#define CONFIG_THREAD_UI_PRIORITY                 (THREAD_IDLE_PRIORITY + 2)
/*************************************************************************************/
/*                                    MODULE TYPE                                    */
/*************************************************************************************/

/*************************************************************************************/
/*                                  PUBLIC FUNCTION                                  */
/*************************************************************************************/

#endif
