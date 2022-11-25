// BLE CURRENT TIME SERVICE

#include "ble_cts.h"
#include "ble.h"
#include <stdint.h>
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gattc_api.h"
#include "esp_log.h"
#include "string.h"
#include <time.h>
// CONSTANT AND MACRO DEFINE
#define BLE_CTS_LOGI(...) ESP_LOGI("BLE_CTS",__VA_ARGS__)
#define INVALID_CHAR_HANDLE             (0xffff)
#define BLE_CTS_SERVICE_UUID            (0x1805)
#define BLE_CTS_CURRENT_TIME_CHAR_UUID  (0x2A2B)

// MODULE VARIABLE
static struct
{
  struct
  {
    esp_bt_uuid_t cts_uuid;
    uint16_t start_handle;
    uint16_t end_handle;
    ble_char_t current_time_char;
  } cts_service;
  uint8_t is_discovered;
} ble_cts =
{
  .cts_service = {.cts_uuid = {.len = ESP_UUID_LEN_16, .uuid.uuid16 = BLE_CTS_SERVICE_UUID},
                  .start_handle = INVALID_CHAR_HANDLE,
                  .end_handle   = INVALID_CHAR_HANDLE,
                  .current_time_char = {.uuid.len = ESP_UUID_LEN_16, 
                                        .uuid.uuid.uuid16 = BLE_CTS_CURRENT_TIME_CHAR_UUID},
                 },
};

// PRIVATE FUNCTIONS PTOTOTYPE
static void ble_cts_gattc_cb(ble_evt_t evt);
static void ble_cts_gap_cb(ble_evt_t evt);
static void ble_cts_start_service_discover();
static uint8_t ble_cts_start_char_discover(ble_char_t * discovering_char, uint8_t is_subcribe);
static void ble_cts_start_descriptor_discover(ble_char_t * discovering_char);
static void ble_cts_parse_current_time(uint8_t *buf, size_t buf_len);
// PUBLIC FUNCTIONS
void ble_cts_init()
{
  BLE_CTS_LOGI("BLE CTS INIT");
  ble_gattc_register_cb(ble_cts_gattc_cb);
  ble_gap_register_cb(ble_cts_gap_cb);
}

// PRIVATE FUNCTIONS
static void ble_cts_gattc_cb(ble_evt_t evt)
{
  esp_gattc_cb_event_t      event = evt.gattc_evt; 
  esp_ble_gattc_cb_param_t *param = evt.gattc_param;
  switch(event)
  {
    case ESP_GATTC_SEARCH_RES_EVT: 
    {
      if ((param->search_res.srvc_id.uuid.len == ble_cts.cts_service.cts_uuid.len) && 
          memcmp(param->search_res.srvc_id.uuid.uuid.uuid128, ble_cts.cts_service.cts_uuid.uuid.uuid128, 2) == 0) 
      {
        BLE_CTS_LOGI("Found CTS service, handle:0x%x - 0x%x", param->search_res.start_handle, param->search_res.end_handle);
        ble_cts.cts_service.start_handle = param->search_res.start_handle;
        ble_cts.cts_service.end_handle = param->search_res.end_handle;
      }
      break;
    }
    case ESP_GATTC_SEARCH_CMPL_EVT:
    {
      // Check if we success discover CTS
      if(ble_cts.cts_service.start_handle == INVALID_CHAR_HANDLE || ble_cts.cts_service.end_handle == INVALID_CHAR_HANDLE) break;
      // Begin discover characteristic
      ble_cts.is_discovered = ble_cts_start_char_discover(&ble_cts.cts_service.current_time_char, true);
      if(ble_cts.is_discovered)
      {
        esp_ble_gattc_read_char (ble_get_gattc_if(),
                                 ble_get_gattc_conn_id(),
                                 ble_cts.cts_service.current_time_char.handle,
                                 ESP_GATT_AUTH_REQ_NONE);
      }
      break;
    }
    case ESP_GATTC_REG_FOR_NOTIFY_EVT:
    {
      if(param->reg_for_notify.handle == ble_cts.cts_service.current_time_char.handle)
      {
        ble_cts_start_descriptor_discover(&ble_cts.cts_service.current_time_char);
      }
      break;
    }
    case ESP_GATTC_NOTIFY_EVT:
    {
      if (param->notify.handle == ble_cts.cts_service.current_time_char.handle)
      {
        BLE_CTS_LOGI("CURRENT TIME NOTIFY");
        ble_cts_parse_current_time(param->notify.value, param->notify.value_len);
      }
      break;
    }
    case ESP_GATTC_READ_CHAR_EVT:
    {
      if(param->read.handle == ble_cts.cts_service.current_time_char.handle)
      {
        ble_cts_parse_current_time(param->read.value, param->read.value_len);
      }
      break;
    }
    default:
      break;
  }
}

static void ble_cts_gap_cb(ble_evt_t evt)
{
  esp_gap_ble_cb_event_t gap_evt = evt.gap_evt;
  esp_ble_gap_cb_param_t * param = evt.gap_param;
  switch(gap_evt)
  {
    case ESP_GAP_BLE_AUTH_CMPL_EVT:
      // Begin discover service
      // BLE_CTS_LOGI("BLE CTS AUTH CMPL");
      ble_cts_start_service_discover();
      break;
    default:
      break;
  }
}

static void ble_cts_start_service_discover()
{
  BLE_CTS_LOGI("Start discovering CTS");
  esp_ble_gattc_search_service(ble_get_gattc_if(), ble_get_gattc_conn_id(), &ble_cts.cts_service.cts_uuid);
}

/**
 * @brief 
 * 
 * @param discovering_char 
 * @param is_subcribe 
 * @return true if found char 
 */
static uint8_t ble_cts_start_char_discover(ble_char_t * discovering_char, uint8_t is_subcribe)
{
  BLE_CTS_LOGI("Start discovering char len:%d", discovering_char->uuid.len);
  uint16_t attr_count  = 0;
  uint16_t offset = 0;
  uint8_t  ret = false;
  esp_ble_gattc_get_attr_count (ble_get_gattc_if(),
                                ble_get_gattc_conn_id(),
                                ESP_GATT_DB_CHARACTERISTIC,
                                ble_cts.cts_service.start_handle,
                                ble_cts.cts_service.end_handle,
                                0x0,
                                &attr_count);
  if(attr_count == 0)
  {
    // fail to discover this service, should not happen
    BLE_CTS_LOGI("Fail to discovering char, attr count:%d", attr_count);
    return false;
  }
  BLE_CTS_LOGI("Found %d attribute in service", attr_count);
  esp_gattc_char_elem_t * char_elem_result = (esp_gattc_char_elem_t *)malloc(sizeof(esp_gattc_char_elem_t) * attr_count);
  if(!char_elem_result){BLE_CTS_LOGI("Fail to allocate memory"); return false;}

  memset(char_elem_result, 0x00, sizeof(esp_gattc_char_elem_t) * attr_count);

  // Begin get all char
  esp_ble_gattc_get_all_char( ble_get_gattc_if(),
                              ble_get_gattc_conn_id(),
                              ble_cts.cts_service.start_handle,
                              ble_cts.cts_service.end_handle,
                              char_elem_result,
                              &attr_count,
                              offset);
  if(!attr_count){BLE_CTS_LOGI("Get All Char fail");free(char_elem_result);return false; }
  // find our char in the list
  for(int i = 0; i < attr_count; i++)
  {
    if((char_elem_result[i].uuid.len == discovering_char->uuid.len) &&
       (memcmp(char_elem_result[i].uuid.uuid.uuid128, discovering_char->uuid.uuid.uuid128, discovering_char->uuid.len) == 0))
    {
      // Found our char
      BLE_CTS_LOGI("Found discovering char, handle:0x%x",char_elem_result[i].char_handle);
      // Store char handle
      discovering_char->handle = char_elem_result[i].char_handle;
      // Store char properties
      discovering_char->prop   = char_elem_result[i].properties;
      if(char_elem_result[i].properties & ESP_GATT_CHAR_PROP_BIT_NOTIFY) BLE_CTS_LOGI("Char:0x%x support notify", char_elem_result[i].char_handle);
      if(char_elem_result[i].properties & ESP_GATT_CHAR_PROP_BIT_INDICATE) BLE_CTS_LOGI("Char:0x%x support indicate", char_elem_result[i].char_handle);
      if(is_subcribe)
      {
        esp_ble_gattc_register_for_notify (ble_get_gattc_if(),
                                           ble_get_gattc_remote_bda(),
                                           discovering_char->handle);
      }
      ret = true;
      break;
    }
  }
  free(char_elem_result);
  return ret;
}

static void ble_cts_start_descriptor_discover(ble_char_t * discovering_char)
{
  BLE_CTS_LOGI("Start discovering Descriptor char len:%d", discovering_char->uuid.len);
  uint16_t desc_count = 0;
  uint16_t cccd_value;

  esp_ble_gattc_get_attr_count (ble_get_gattc_if(),
                                ble_get_gattc_conn_id(),
                                ESP_GATT_DB_DESCRIPTOR,
                                ble_cts.cts_service.start_handle,
                                ble_cts.cts_service.end_handle,
                                discovering_char->handle,
                                &desc_count);
  if(!desc_count){BLE_CTS_LOGI("Found no Descriptor"); return;}
  BLE_CTS_LOGI("Found %d descriptor", desc_count);
  // allocate memory to store result
  esp_gattc_descr_elem_t * descr_elem_result = malloc(sizeof(esp_gattc_descr_elem_t) * desc_count);
  if (!descr_elem_result) {BLE_CTS_LOGI("Fail to allocate memory"); return;}
  // Get all desc for this char
  esp_ble_gattc_get_all_descr(ble_get_gattc_if(),
                              ble_get_gattc_conn_id(),
                              discovering_char->handle,
                              descr_elem_result,
                              &desc_count,
                              0);
  // Find CCCD
  for(int i = 0; i < desc_count; i++)
  {
    if (descr_elem_result[i].uuid.len == ESP_UUID_LEN_16 && descr_elem_result[i].uuid.uuid.uuid16 == ESP_GATT_UUID_CHAR_CLIENT_CONFIG)
    {
      // Store cccd handle
      discovering_char->cccd_handle = descr_elem_result[i].handle;
      BLE_CTS_LOGI("found CCCD, handle:0x%x", descr_elem_result[i].handle);
      // Subcribe to char
      if((discovering_char->prop) & ESP_GATT_CHAR_PROP_BIT_NOTIFY) cccd_value = 0x01;
      else cccd_value = 0x02;
      esp_ble_gattc_write_char_descr (ble_get_gattc_if(),
                                      ble_get_gattc_conn_id(),
                                      discovering_char->cccd_handle,
                                      sizeof(cccd_value),
                                      (uint8_t *)&cccd_value,
                                      ESP_GATT_WRITE_TYPE_NO_RSP,
                                      ESP_GATT_AUTH_REQ_NONE);
      break;
    }
  }
  free(descr_elem_result);
}

static void ble_cts_parse_current_time(uint8_t *buf, size_t buf_len)
{
  if(!buf || buf_len == 0) return;
  struct __attribute__((__packed__))
  {
    uint16_t year;
    uint8_t mon;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t dow;
    uint8_t dummy[2];
  } current_time;
  memcpy(&current_time, buf, sizeof(current_time) < buf_len ? sizeof(current_time) : buf_len);
  BLE_CTS_LOGI("Current day: %d/%d/%d", current_time.year, current_time.mon, current_time.day);
  BLE_CTS_LOGI("Current time: %d:%d:%d", current_time.hour, current_time.min, current_time.sec);
}