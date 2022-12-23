
#include "ble.h"
#include <stdint.h>
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gattc_api.h"
#include "esp_log.h"
#include "string.h"
#include "ble_svc_dis.h"
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ui.h"
#include "ui_def.h"
#define BLE_AMS_LOGI(...) ESP_LOGI("BLE_CTS",__VA_ARGS__)


static void ble_cts_gattc_cb(ble_evt_t evt);
static void ble_cts_gap_cb(ble_evt_t evt);

void ble_ams_init()
{
  ble_gattc_register_cb(ble_cts_gattc_cb);
  ble_gap_register_cb(ble_cts_gap_cb);
}