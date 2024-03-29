
// #include "ble.h"
// #include <stdint.h>
// #include "esp_gap_ble_api.h"
// #include "esp_gatts_api.h"
// #include "esp_bt_defs.h"
// #include "esp_bt_main.h"
// #include "esp_gattc_api.h"
// #include "esp_log.h"
// #include "string.h"
// #include "ble_svc_dis.h"
// #include <time.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "ui.h"
// #include "ui_def.h"
// #define BLE_AMS_LOGI(...) ESP_LOGI("BLE_CTS",__VA_ARGS__)

// // static const uint8_t AMS_SERVICE_UUID[] = {0xDC, 0xF8, 0x55, 0xAD, 0x02, 0xC5, 0xF4, 0x8E, 0x3A, 0x43, 0x36, 0x0F, 0x2B, 0x50, 0xD3, 0x89};
// // static const uint8_t AMS_CHAR_REMOTE_CMD_UUID[] = {0xC2, 0x51, 0xCA, 0xF7, 0x56, 0x0E, 0xDF, 0xB8, 0x8A, 0x4A, 0xB1, 0x57, 0xD8, 0x81, 0x3C, 0x9B};
// // static const uint8_t AMS_CHAR_ENTITY_UPDATE_UUID[] = {0x02, 0xC1, 0x96, 0xBA, 0x92, 0xBB, 0x0C, 0x9A, 0x1F, 0x41, 0x8D, 0x80, 0xCE, 0xAB, 0x7C, 0x2F};
// // static const uint8_t AMS_CHAR_ENTITY_ATTRIBUTE_UUID[] = {0xD7, 0xD5, 0xBB, 0x70, 0xA8, 0xA3, 0xAB, 0xA6, 0xD8, 0x46, 0xAB, 0x23, 0x8C, 0xF3, 0xB2, 0xC6};
// static void ble_cts_gattc_cb(ble_evt_t evt);
// static void ble_cts_gap_cb(ble_evt_t evt);

// void ble_ams_init()
// {
//   ble_gattc_register_cb(ble_cts_gattc_cb);
//   ble_gap_register_cb(ble_cts_gap_cb);
// }