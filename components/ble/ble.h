#ifndef _BLE_H
#define _BLE_H
#include "std_type.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gattc_api.h"
#include "esp_gatts_api.h"
/*************************************************************************************/
/*                                  CONSTANT DEFINE                                  */
/*************************************************************************************/
#define SERVICE_DECLARE(p_uuid, uuid_length) {.attr_control = ESP_GATT_AUTO_RSP,\
                           .att_desc = {\
                              .uuid_length  = ESP_UUID_LEN_16,\
                              .uuid_p       = &(((uint16_t[1]){ESP_GATT_UUID_PRI_SERVICE})),\
                              .perm         = ESP_GATT_PERM_READ,\
                              .max_length   = uuid_length,\
                              .length       = uuid_length,\
                              .value        = p_uuid,\
                           }\
/*************************************************************************************/
/*                                    MODULE TYPE                                    */
/*************************************************************************************/
typedef enum
{
  BLE_STATE_UNKNOW,
  BLE_STATE_ADVERTISING,
  BLE_STATE_CONNECTED,
  BLE_STATE_PAIRED
} ble_state_t;

typedef struct
{
  union
  {
    esp_gattc_cb_event_t    gattc_evt;
    esp_gap_ble_cb_event_t  gap_evt;
    esp_gatts_cb_event_t    gatts_evt;
  };
  union
  {
    esp_ble_gattc_cb_param_t  * gattc_param;
    esp_ble_gap_cb_param_t    * gap_param;
    esp_ble_gatts_cb_param_t  * gatts_param;
  };
} ble_evt_t;

// typedef struct
// {
//   esp_bt_uuid_t char_uuid;
//   uint16_t handle;
//   uint16_t cccd_handle;
//   uint8_t  prop;
//   uint8_t  is_subcribed;
//   uint8_t  need_subcribe;
// } ble_char_t;

// typedef struct
// {
//   esp_bt_uuid_t svc_uuid;
//   uint16_t start_handle;
//   uint16_t end_handle;
//   ble_char_t * char_list;
//   uint16_t num_char;
//   char * svc_name;
// } ble_svc_t;

typedef void (*ble_evt_cb_t)(ble_evt_t evt);
/*************************************************************************************/
/*                                  PUBLIC FUNCTION                                  */
/*************************************************************************************/
void ble_init();
void ble_gattc_register_cb(ble_evt_cb_t cb);
void ble_gap_register_cb(ble_evt_cb_t cb);
void ble_gatts_register_cb(ble_evt_cb_t cb);

uint8_t * ble_get_gattc_remote_bda();
uint16_t ble_get_gattc_if();
uint16_t ble_get_gattc_conn_id();
ble_state_t ble_get_current_state();
uint16_t ble_get_gatts_if();
#endif