#include "ble_adv.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_gatt_common_api.h"
#include "ble_config.h"
#include "esp_log.h"
#include "ble.h"
#define BLE_ADV_LOG(...) printf(__VA_ARGS__)
static esp_ble_adv_data_t adv_config = {
  .set_scan_rsp = false,
  .include_txpower = false,
  .min_interval = 12, //slave connection min interval, Time = min_interval * 1.25 msec
  .max_interval = 72, //slave connection max interval, Time = max_interval * 1.25 msec
  .appearance = ESP_BLE_APPEARANCE_GENERIC_WATCH,
  .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};
// config scan response data
static esp_ble_adv_data_t scan_rsp_config = {
    .set_scan_rsp = true,
    .include_name = true,
    .manufacturer_len = 0,
    .p_manufacturer_data = NULL,
}; 
static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = CONFIG_ADV_INTERVAL, 
    .adv_int_max        = CONFIG_ADV_INTERVAL,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_RANDOM,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static void ble_adv_gap_cb(ble_evt_t evt);
static void ble_adv_gattc_cb(ble_evt_t evt);

void ble_adv_init()
{
  //generate a resolvable random address
  esp_ble_gap_config_local_privacy(true);
  ble_gap_register_cb(ble_adv_gap_cb);
  ble_gattc_register_cb(ble_adv_gattc_cb);

  esp_ble_gap_set_device_name(CONFIG_BLE_NAME);
  esp_ble_gap_config_local_icon(ESP_BLE_APPEARANCE_GENERIC_WATCH);
  esp_ble_gap_config_adv_data(&adv_config);
  esp_ble_gap_config_adv_data(&scan_rsp_config);

}

void ble_adv_start()
{
  esp_ble_gap_start_advertising(&adv_params);
  BLE_ADV_LOG("Start Advertising\r\n");
}

static void ble_adv_gap_cb(ble_evt_t evt)
{
  esp_gap_ble_cb_event_t event  = evt.gap_evt; 
  esp_ble_gap_cb_param_t *param = evt.gap_param;
  switch(event)
  {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
      ble_adv_start();
      break;
    default:
      break;
  }
}

static void ble_adv_gattc_cb(ble_evt_t evt)
{
  esp_gattc_cb_event_t      event = evt.gattc_evt; 
  esp_ble_gattc_cb_param_t *param = evt.gattc_param;
  //BLE_ADV_LOG("gattc evt:%d\r\n", event);
  switch(event)
  {
    case ESP_GATTC_DISCONNECT_EVT:
      ble_adv_start(); // start adv when disconnnect
      break;
    default:
      break;
  }
}