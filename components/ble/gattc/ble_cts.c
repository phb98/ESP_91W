//###########################################################################################################
//      Includes
//###########################################################################################################
#define MODULE_NAME BLE_CTS
#include "ble_log.h"
#include "ble_def.h"
#include "ble_disc.h"
#include "ble_cts.h"
//###########################################################################################################
//      Constant #Defines
#define CTS_SERVICE_UUID 0x1805

//###########################################################################################################
// MODULE VARIABLES
static ble_char_t cts_chars[]; // array of characteristics
static struct 
{
  ble_svc_t cts_service;
} ble_cts =
{
  .cts_service = 
  {
    .svc_name  = "Current Time Service",
    .svc_uuid  = BLE_UUID_DECLARE_16(CTS_SERVICE_UUID),
    .start_hdl = BLE_INVALID_CHAR_HDL,
    .end_hdl   = BLE_INVALID_CHAR_HDL,
    .char_list = NULL,
  }
};
//###########################################################################################################
//      Private Function Prototypes
//###########################################################################################################
static void ble_cts_cb();
//###########################################################################################################
//      Public Functions
//###########################################################################################################
void ble_cts_init()
{
  ble_disc_request(&ble_cts.cts_service, ble_cts_cb);
}

//###########################################################################################################
//      Private Functions
//###########################################################################################################
static void ble_cts_cb()
{
}
// ble_cts_time_t ble_cts_get_time() 
// {
//   // Update current time
//   ble_cts_update_time_from_tick();
//   return (ble_cts.current_time);
// }
// // PRIVATE FUNCTIONS
// static void ble_cts_gattc_cb(ble_evt_t evt)
// {
//   esp_gattc_cb_event_t      event = evt.gattc_evt; 
//   esp_ble_gattc_cb_param_t *param = evt.gattc_param;
//   switch(event)
//   {
//     case ESP_GATTC_NOTIFY_EVT:
//     {
//       if (param->notify.handle == ble_cts.cts_service.char_list[CTS_CURRENT_TIME_CHAR].handle)
//       {
//         BLE_CTS_LOGI("CURRENT TIME NOTIFY");
//         ble_cts_parse_current_time(param->notify.value, param->notify.value_len);
//       }
//       break;
//     }
//     case ESP_GATTC_READ_CHAR_EVT:
//     {
//       if(param->read.handle == ble_cts.cts_service.char_list[CTS_CURRENT_TIME_CHAR].handle)
//       {
//         ble_cts_parse_current_time(param->read.value, param->read.value_len);
//       }
//       break;
//     }
//     case ESP_GATTC_DISCONNECT_EVT:
//     {
//       ble_cts.is_discovered = false;
//       ble_cts_service_reset();
//       BLE_CTS_LOGI("Service closed");
//       break;
//     }
//     default:
//       break;
//   }
// }

// static void ble_cts_gap_cb(ble_evt_t evt)
// {
//   esp_gap_ble_cb_event_t gap_evt = evt.gap_evt;
//   esp_ble_gap_cb_param_t * param = evt.gap_param;
//   switch(gap_evt)
//   {
//     case ESP_GAP_BLE_AUTH_CMPL_EVT:
//       // Begin discover service
//       // BLE_CTS_LOGI("BLE CTS AUTH CMPL");
//       if(!ble_cts.is_discovered && param->ble_security.auth_cmpl.success) ble_cts_start_service_discover();
//       break;
//     default:
//       break;
//   }
// }


// static void ble_cts_parse_current_time(uint8_t *buf, size_t buf_len)
// {
//   if(!buf || buf_len == 0) return;
//   // Update current date time
//   memcpy(&ble_cts.current_time, buf, sizeof(ble_cts_time_t) < buf_len ? sizeof(ble_cts_time_t) : buf_len);
//   ble_cts.tick_when_time_update = xTaskGetTickCount();
//   BLE_CTS_LOGI("Current day: %d/%d/%d", ble_cts.current_time.year, ble_cts.current_time.mon, ble_cts.current_time.day);
//   BLE_CTS_LOGI("Current time: %d:%d:%d", ble_cts.current_time.hour, ble_cts.current_time.min, ble_cts.current_time.sec);
//   // Post it to ui
//   ui_input_t time_update = 
//   {
//     .evt = UI_EVT_TIME_UPDATE,
//     .param.time_update.year  =  ble_cts.current_time.year,
//     .param.time_update.month =  ble_cts.current_time.mon,
//     .param.time_update.day   =  ble_cts.current_time.day,
//     .param.time_update.hour  =  ble_cts.current_time.hour,
//     .param.time_update.min   =  ble_cts.current_time.min,
//     .param.time_update.sec   =  ble_cts.current_time.sec,
//     .param_length            =  sizeof(ui_time_update_t)
//   };
//   ui_input_handle(&time_update);
// }


// static void ble_cts_update_time_from_tick()
// {
//   uint64_t delta_tick = xTaskGetTickCount() - ble_cts.tick_when_time_update;
//   uint64_t delta_ms = delta_tick * (portTICK_PERIOD_MS);
//   uint64_t delta_s = delta_ms / 1000;
//   if(delta_s > 0)
//   {
//     struct tm current_time = 
//     {
//       .tm_hour = ble_cts.current_time.hour,
//       .tm_min  = ble_cts.current_time.min,
//       .tm_sec  = ble_cts.current_time.sec,
//       .tm_year = ble_cts.current_time.year - 1900,
//       .tm_mon  = ble_cts.current_time.mon - 1,
//       .tm_mday = ble_cts.current_time.day,
//       .tm_wday = ble_cts.current_time.dow,
//       .tm_isdst = -1,
//     };
//     time_t current_epoch = mktime(&current_time) + (time_t)delta_s;
//     struct tm * update_time = gmtime(&current_epoch);
//     // convert back to our formet 
//     ble_cts.current_time.hour = update_time->tm_hour;
//     ble_cts.current_time.min  = update_time->tm_min;  
//     ble_cts.current_time.sec  = update_time->tm_sec;   
//     ble_cts.current_time.year = update_time->tm_year + 1900;
//     ble_cts.current_time.mon  = update_time->tm_mon  + 1;
//     ble_cts.current_time.day  = update_time->tm_mday;
//     ble_cts.current_time.dow  = update_time->tm_wday; 
//     ble_cts.tick_when_time_update = xTaskGetTickCount();
//     BLE_CTS_LOGI("Current day: %d/%d/%d", ble_cts.current_time.year, ble_cts.current_time.mon, ble_cts.current_time.day);
//     BLE_CTS_LOGI("Current time: %d:%d:%d", ble_cts.current_time.hour, ble_cts.current_time.min, ble_cts.current_time.sec);
//   }
// }


// static void ble_cts_start_service_discover()
// {
//   if(!ble_cts.is_discovered)
//   {
//     BLE_CTS_LOGI("Request discovering CTS service");
//     static ble_svc_dis_t cts_svc_dis = 
//     {
//       .request_svc = &ble_cts.cts_service,
//       .cb = ble_cts_service_discover_cb
//     };
//     ble_svc_dis_request(cts_svc_dis);
//   }
// }

// static void ble_cts_service_discover_cb(ble_svc_dis_status_t status)
// {
//   if(status == BLE_SVC_DIS_SUCCESS)
//   {
//     BLE_CTS_LOGI("Discovering CTS service success");
//     ble_cts.is_discovered = true;
//     // Get current Date time from gatt server
//     esp_ble_gattc_read_char (ble_get_gattc_if(),
//                               ble_get_gattc_conn_id(),
//                               ble_cts.cts_service.char_list[CTS_CURRENT_TIME_CHAR].handle,
//                               ESP_GATT_AUTH_REQ_NONE);
//   }
//   else
//   {
//     BLE_CTS_LOGI("Discovering CTS service fail, reason:%d", status);
//   }
// }

// static void ble_cts_service_reset()
// {
//   ble_svc_t * svc = &(ble_cts.cts_service);
//   ble_svc_dis_reset(svc);
// }
