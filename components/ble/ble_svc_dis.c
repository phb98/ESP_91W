// #include "ble_svc_dis.h"
// #include <stdint.h>
// #include "ble.h"
// #include "esp_gap_ble_api.h"
// #include "esp_gatts_api.h"
// #include "esp_bt_defs.h"
// #include "esp_bt_main.h"
// #include "esp_gattc_api.h"
// #include "esp_log.h"
// #include <string.h>
// #include "thread_config.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/queue.h"
// #include "freertos/event_groups.h"

// /*************************************************************************************/
// /*                                  CONSTANT DEFINE                                  */
// /*************************************************************************************/
// #define MAX_DISCOVERINNG_SVC    (16)
// #define BLE_INVALID_HANDLE      (0)
// #define BLE_SVC_DIS_LOGI(...)   ESP_LOGI("BLE_SVC_DIS", __VA_ARGS__);
// #define IS_PROP_SUBCRIABLE(p)   (((p) & ESP_GATT_CHAR_PROP_BIT_NOTIFY) || ((p) & ESP_GATT_CHAR_PROP_BIT_INDICATE))      
// /*************************************************************************************/
// /*                                    MODULE TYPE                                    */
// /*************************************************************************************/
// typedef enum
// {
//   EVENT_START_DISCOVER  = 1 << 0,
//   EVENT_START_LIST_CHAR = 1 << 1,
//   EVENT_START_LIST_DESC = 1 << 2,
//   EVENT_START_SUBCRIBING= 1 << 3,
//   EVENT_SVC_FOUND       = 1 << 4,
//   EVENT_BLE_DISCONNECT  = 1 << 5,
// } ble_svc_dis_event_t;
// /*************************************************************************************/
// /*                                  MODULE VARIABLE                                  */
// /*************************************************************************************/
// static struct
// {
//   ble_svc_dis_t discovering_service;
//   QueueHandle_t queue_handle;
//   TaskHandle_t  task_handle;
//   StaticTask_t  task_buffer;
//   uint8_t       task_stack[CONFIG_THREAD_BLE_SVC_DIS_STACK_SIZE];
//   EventGroupHandle_t event_flag;
//   uint8_t       is_discovering;
//   uint8_t       is_connected;
// } ble_svc_dis = 
// {
//   .is_connected = false,
// };
// /*************************************************************************************/
// /*                            PRIVATE FUNCTION PROTOTYPE                             */
// /*************************************************************************************/
// static void ble_svc_dis_gattc_cb(ble_evt_t evt);
// static void ble_svc_dis_gap_cb(ble_evt_t evt);
// static void ble_svc_dis_thread_entry(void * param);
// static void ble_svc_dis_post_event(ble_svc_dis_event_t event);
// static void ble_svc_dis_start();
// static void ble_svc_dis_end(ble_svc_dis_status_t status);
// static void ble_svc_dis_char();
// static void ble_svc_dis_desc();
// static void ble_svc_dis_subcribe();
// static void ble_svc_dis_handle_disconnect();
// /*************************************************************************************/
// /*                                  PUBLIC FUNCTION                                  */
// /*************************************************************************************/
// void ble_svc_dis_init()
// {
//   BLE_SVC_DIS_LOGI("BLE_SVC_DIS_INIT");
//   ble_svc_dis.queue_handle = xQueueCreate(MAX_DISCOVERINNG_SVC, sizeof(ble_svc_dis_t));
//   ble_svc_dis.event_flag = xEventGroupCreate();
//   if(!ble_svc_dis.event_flag) BLE_SVC_DIS_LOGI("Create evt group fail");
//   ble_gap_register_cb(ble_svc_dis_gap_cb);
//   ble_gattc_register_cb(ble_svc_dis_gattc_cb);
//   ble_svc_dis.task_handle  = xTaskCreateStatic(ble_svc_dis_thread_entry,
//                                                CONFIG_THREAD_BLE_SVC_DIS_NAME,
//                                                CONFIG_THREAD_BLE_SVC_DIS_STACK_SIZE,
//                                                NULL,
//                                                CONFIG_THREAD_BLE_SVC_DIS_PRIORITY,
//                                                ble_svc_dis.task_stack,
//                                                &(ble_svc_dis.task_buffer));
// }

// void ble_svc_dis_request(ble_svc_dis_t svc)
// {
//   if(ble_svc_dis.is_connected)
//   {
//     if(!ble_svc_dis.is_discovering)
//     {
//       BLE_SVC_DIS_LOGI("Start discovering: %s", svc.request_svc->svc_name);
//       ble_svc_dis.is_discovering = true;
//       memcpy(&ble_svc_dis.discovering_service, &svc, sizeof(ble_svc_dis_t));
//       ble_svc_dis_post_event(EVENT_START_DISCOVER);
//     }
//     else
//     {
//       BLE_SVC_DIS_LOGI("%s is searching, push %s to queue", ble_svc_dis.discovering_service.request_svc->svc_name, svc.request_svc->svc_name);
//       xQueueSend(ble_svc_dis.queue_handle, &svc, 0);
//     }
//   }
//   else BLE_SVC_DIS_LOGI("Request service discover fail, BLE disconnected");
// }

// void ble_svc_dis_reset(ble_svc_t * svc)
// {
//   BLE_SVC_DIS_LOGI("Reset %s", svc->svc_name);
//   if(svc)
//   {
//     svc->start_handle = BLE_INVALID_HANDLE;
//     svc->end_handle   = BLE_INVALID_HANDLE;
//     for(int i = 0; i < svc->num_char; i++)
//     {
//       svc->char_list[i].cccd_handle = BLE_INVALID_HANDLE;
//       svc->char_list[i].handle = BLE_INVALID_HANDLE;
//       svc->char_list[i].is_subcribed = false;
//       svc->char_list[i].prop = 0;
//     }
//   }
// }
// /*************************************************************************************/
// /*                                 PRIVATE FUNCTION                                  */
// /*************************************************************************************/
// static void ble_svc_dis_thread_entry(void * param)
// {
//   while(1)
//   {
//     uint32_t evt = xEventGroupWaitBits(ble_svc_dis.event_flag, 0xFFFFFF, pdTRUE, pdFALSE, portMAX_DELAY);
//     if(ble_svc_dis.is_connected)
//     {
//       if(evt & EVENT_START_DISCOVER)
//       {
//         // Discover new service
//         ble_svc_dis_start();
//       }
//       else if(evt & EVENT_START_LIST_CHAR)
//       {
//         ble_svc_dis_char();
//       }
//       else if(evt & EVENT_START_LIST_DESC)
//       {
//         ble_svc_dis_desc();
//       }
//       else if(evt & EVENT_START_SUBCRIBING)
//       {
//         ble_svc_dis_subcribe();
//       }
//       else if(evt & EVENT_BLE_DISCONNECT)
//       {
//         ble_svc_dis_handle_disconnect();
//       }
//     }
//   }
// }
// static void ble_svc_dis_gattc_cb(ble_evt_t evt)
// {
//   esp_gattc_cb_event_t      event = evt.gattc_evt; 
//   esp_ble_gattc_cb_param_t *param = evt.gattc_param;
//   switch(event)
//   {
//     case ESP_GATTC_SEARCH_RES_EVT: 
//     {
//       if ((param->search_res.srvc_id.uuid.len == ble_svc_dis.discovering_service.request_svc->svc_uuid.len) && 
//           memcmp(param->search_res.srvc_id.uuid.uuid.uuid128, ble_svc_dis.discovering_service.request_svc->svc_uuid.uuid.uuid128, param->search_res.srvc_id.uuid.len) == 0) 
//       {
//         BLE_SVC_DIS_LOGI("Found %s", ble_svc_dis.discovering_service.request_svc->svc_name);
//         BLE_SVC_DIS_LOGI("Start handle:0x%x, End handle:0x%x", param->search_res.start_handle, param->search_res.end_handle);
//         ble_svc_dis.discovering_service.request_svc->start_handle = param->search_res.start_handle;
//         ble_svc_dis.discovering_service.request_svc->end_handle   = param->search_res.end_handle;
//       }
//       break;
//     }
//     case ESP_GATTC_SEARCH_CMPL_EVT:
//     {
//       if(ble_svc_dis.discovering_service.request_svc->start_handle == BLE_INVALID_HANDLE || 
//          ble_svc_dis.discovering_service.request_svc->end_handle   == BLE_INVALID_HANDLE)
//       {
//         ble_svc_dis_end(BLE_SVC_DIS_FAIL);
//       }
//       else
//       {
//         // Begin discover characteristic
//         ble_svc_dis_post_event(EVENT_START_LIST_CHAR);
//       }
//       break;
//     }
//     case ESP_GATTC_REG_FOR_NOTIFY_EVT:
//     {
//       if(ble_svc_dis.is_discovering)
//       {
//         ble_svc_dis_post_event(EVENT_START_SUBCRIBING);
//       }
//       break;
//     }
//     case ESP_GATTC_DISCONNECT_EVT:
//     {
//       ble_svc_dis_post_event(EVENT_BLE_DISCONNECT);
//       break;
//     }
//     case ESP_GATTC_CONNECT_EVT:
//     {
//       ble_svc_dis.is_connected = true;
//     }
//     default:
//       break;
//   }
// }
// static void ble_svc_dis_gap_cb(ble_evt_t evt)
// {

// }

// static void ble_svc_dis_post_event(ble_svc_dis_event_t event)
// {
//   xEventGroupSetBits(ble_svc_dis.event_flag, event);
// }

// static void ble_svc_dis_start()
// {
//   esp_bt_uuid_t uuid;
//   memcpy(&uuid, &(ble_svc_dis.discovering_service.request_svc->svc_uuid), sizeof(uuid));
//   for(int i = 0; i < ble_svc_dis.discovering_service.request_svc->num_char; i++)
//   {
//     ble_svc_dis.discovering_service.request_svc->char_list[i].handle      = BLE_INVALID_HANDLE;
//     ble_svc_dis.discovering_service.request_svc->char_list[i].cccd_handle = BLE_INVALID_HANDLE;
//   }
//   ble_svc_dis.discovering_service.request_svc->start_handle = BLE_INVALID_HANDLE;
//   ble_svc_dis.discovering_service.request_svc->end_handle = BLE_INVALID_HANDLE;
//   esp_ble_gattc_search_service(ble_get_gattc_if(), ble_get_gattc_conn_id(), &uuid);
// }

// static void ble_svc_dis_char()
// {
//   BLE_SVC_DIS_LOGI("Start listing char in %s", ble_svc_dis.discovering_service.request_svc->svc_name);
//   uint16_t attr_count  = 0;
//   uint16_t offset = 0;
//   esp_ble_gattc_get_attr_count (ble_get_gattc_if(),
//                                 ble_get_gattc_conn_id(),
//                                 ESP_GATT_DB_CHARACTERISTIC,
//                                 ble_svc_dis.discovering_service.request_svc->start_handle,
//                                 ble_svc_dis.discovering_service.request_svc->end_handle,
//                                 0x0,
//                                 &attr_count);
//   if(attr_count == 0)
//   {
//     // fail to discover this service, should not happen
//     BLE_SVC_DIS_LOGI("Fail to lis char, attr count:%d", attr_count);
//     ble_svc_dis_end(BLE_SVC_DIS_FAIL);
//   }
//   else
//   {
//     BLE_SVC_DIS_LOGI("Found %d char", attr_count);
//     esp_gattc_char_elem_t * char_elem_result = (esp_gattc_char_elem_t *)malloc(sizeof(esp_gattc_char_elem_t) * attr_count);
//     if(!char_elem_result){BLE_SVC_DIS_LOGI("Fail to allocate memory"); return; }

//     memset(char_elem_result, 0x00, sizeof(esp_gattc_char_elem_t) * attr_count);

//     // Begin get all char
//     esp_ble_gattc_get_all_char( ble_get_gattc_if(),
//                                 ble_get_gattc_conn_id(),
//                                 ble_svc_dis.discovering_service.request_svc->start_handle,
//                                 ble_svc_dis.discovering_service.request_svc->end_handle,
//                                 char_elem_result,
//                                 &attr_count,
//                                 offset);
//     // find our char in the list
//     for(int char_idx = 0; char_idx < ble_svc_dis.discovering_service.request_svc->num_char; char_idx++)
//     {
//       ble_char_t * processing_char = &(ble_svc_dis.discovering_service.request_svc->char_list[char_idx]);
//       for(int i = 0; i < attr_count; i++)
//       {
//         if((char_elem_result[i].uuid.len == processing_char->char_uuid.len) &&
//           (memcmp(char_elem_result[i].uuid.uuid.uuid128, processing_char->char_uuid.uuid.uuid128, processing_char->char_uuid.len) == 0))
//         {
//           // Found our char
//           BLE_SVC_DIS_LOGI("Found char %d, handle:0x%x",char_idx, char_elem_result[i].char_handle);
//           // Store char handle
//           processing_char->handle = char_elem_result[i].char_handle;
//           // Store char properties
//           processing_char->prop   = char_elem_result[i].properties;
//           if(char_elem_result[i].properties & ESP_GATT_CHAR_PROP_BIT_NOTIFY)   BLE_SVC_DIS_LOGI("Char:0x%x support notify", char_elem_result[i].char_handle);
//           if(char_elem_result[i].properties & ESP_GATT_CHAR_PROP_BIT_INDICATE) BLE_SVC_DIS_LOGI("Char:0x%x support indicate", char_elem_result[i].char_handle);
//           break;
//         }
//       }
//     }
//     free(char_elem_result);
//     // begin discovering descriptor
//     ble_svc_dis_post_event(EVENT_START_LIST_DESC);
//   }
//   return;
// }
// static void ble_svc_dis_desc()
// {
//   BLE_SVC_DIS_LOGI("Start listing descriptor in %s", ble_svc_dis.discovering_service.request_svc->svc_name);

//   uint16_t desc_count;
//   uint8_t  need_subcribe = false;
//   ble_svc_t * processing_svc = ble_svc_dis.discovering_service.request_svc;
//   for(int char_idx = 0; char_idx < processing_svc->num_char; char_idx++)
//   {
//     desc_count = 0;
//     ble_char_t * processing_char = &(processing_svc->char_list[char_idx]);
//     if(processing_char->handle == BLE_INVALID_HANDLE) continue;

//     esp_ble_gattc_get_attr_count (ble_get_gattc_if(),
//                                   ble_get_gattc_conn_id(),
//                                   ESP_GATT_DB_DESCRIPTOR,
//                                   processing_svc->start_handle,
//                                   processing_svc->end_handle,
//                                   processing_char->handle,
//                                   &desc_count);
//     if(!desc_count){BLE_SVC_DIS_LOGI("Found no Descriptor"); continue;}
//     BLE_SVC_DIS_LOGI("Found %d descriptor for handle:0x%x", desc_count, processing_char->handle);
//     // allocate memory to store result
//     esp_gattc_descr_elem_t * descr_elem_result = malloc(sizeof(esp_gattc_descr_elem_t) * desc_count);
//     if (!descr_elem_result) {BLE_SVC_DIS_LOGI("Fail to allocate memory"); continue;}
//     // Get all desc for this char
//     esp_ble_gattc_get_all_descr(ble_get_gattc_if(),
//                                 ble_get_gattc_conn_id(),
//                                 processing_char->handle,
//                                 descr_elem_result,
//                                 &desc_count,
//                                 0);
//     // Find CCCD
//     for(int i = 0; i < desc_count; i++)
//     {
//       if (descr_elem_result[i].uuid.len == ESP_UUID_LEN_16 && descr_elem_result[i].uuid.uuid.uuid16 == ESP_GATT_UUID_CHAR_CLIENT_CONFIG)
//       {
//         // Store cccd handle
//         processing_char->cccd_handle = descr_elem_result[i].handle;
//         BLE_SVC_DIS_LOGI("found CCCD, handle:0x%x for char handle:0x%x", descr_elem_result[i].handle, processing_char->handle);
//         if(processing_char->need_subcribe && IS_PROP_SUBCRIABLE(processing_char->prop))
//         {
//           need_subcribe = true;
//           esp_ble_gattc_register_for_notify(ble_get_gattc_if(),
//                                             ble_get_gattc_remote_bda(),
//                                             processing_char->handle);
//         } 
//       }
//     }
//     free(descr_elem_result);
//   }
//   if(!need_subcribe)
//   {
//     BLE_SVC_DIS_LOGI("Service dont/cant subcribed");
//     ble_svc_dis_end(BLE_SVC_DIS_SUCCESS);
//   }
// }

// static void ble_svc_dis_end(ble_svc_dis_status_t status)
// {
//   BLE_SVC_DIS_LOGI("Finish discovering %s, status:%d", ble_svc_dis.discovering_service.request_svc->svc_name, status);
//   if(ble_svc_dis.discovering_service.cb)
//   {
//     ble_svc_dis.discovering_service.cb(status);
//     // set cb to NULL to prevent call callback multi time
//     ble_svc_dis.discovering_service.cb = NULL;
//   }
//   // Check if there is pending searching service
//   ble_svc_dis.is_discovering = false;
//   ble_svc_dis_t pending_svc;
//   if(xQueueReceive(ble_svc_dis.queue_handle, &pending_svc, 0x0) == pdTRUE)
//   {
//     BLE_SVC_DIS_LOGI("Push %s from queue to discover", pending_svc.request_svc->svc_name);
//     ble_svc_dis_request(pending_svc);
//   }
//   else BLE_SVC_DIS_LOGI("No more service to discover");
// }

// static void ble_svc_dis_subcribe()
// {
//   BLE_SVC_DIS_LOGI("Begin subcribing");
//   ble_char_t * processing_char;
//   ble_svc_t *  processing_svc = ble_svc_dis.discovering_service.request_svc;
//   uint16_t     cccd_value;
//   for(int char_idx = 0; char_idx < processing_svc->num_char; char_idx++)
//   {
//     processing_char = &(processing_svc->char_list[char_idx]);
//     if(processing_char->need_subcribe && 
//        !(processing_char->is_subcribed) && 
//        processing_char->cccd_handle != BLE_INVALID_HANDLE)
//     {
//       BLE_SVC_DIS_LOGI("Subcribe to char handle:0x%x", processing_char->handle)
//       if((processing_char->prop) & ESP_GATT_CHAR_PROP_BIT_NOTIFY) cccd_value = 0x01;
//       else cccd_value = 0x02;
//       BLE_SVC_DIS_LOGI("cccd value:%d", cccd_value);
//       BLE_SVC_DIS_LOGI("cccd handle:0x%x", processing_char->cccd_handle);
//       esp_ble_gattc_write_char (ble_get_gattc_if(),
//                                 ble_get_gattc_conn_id(),
//                                 processing_char->cccd_handle,
//                                 sizeof(cccd_value),
//                                 (uint8_t*)&cccd_value,
//                                 ESP_GATT_WRITE_TYPE_NO_RSP,
//                                 ESP_GATT_AUTH_REQ_NONE);
//       processing_char->is_subcribed = true;
//     }
//   }
//   ble_svc_dis_end(BLE_SVC_DIS_SUCCESS); // finish discovering service
// }

// static void ble_svc_dis_handle_disconnect()
// {
//   BLE_SVC_DIS_LOGI("ble_svc_dis handling disconnect");
//   if(ble_svc_dis.is_discovering)
//   {
//     // Stop immediately by calling callback
//     ble_svc_dis_end(BLE_SVC_DIS_DISCONNECTED);
//     ble_svc_dis.is_connected = false;
//     // Calling other callback
//     ble_svc_dis_t pending_svc;
//     // pop ultil queue empty
//     while(xQueueReceive(ble_svc_dis.queue_handle, &pending_svc, 0x0) == pdTRUE)
//     {
//       if(pending_svc.cb) pending_svc.cb(BLE_SVC_DIS_DISCONNECTED);
//     }
//   }
// }

