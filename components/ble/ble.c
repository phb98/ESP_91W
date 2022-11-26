#include "ble.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_gatt_common_api.h"
#include "ble_config.h"
#include "ble_adv.h"
#include <stdint.h>
#include "ble_cts.h"
#include "ble_svc_dis.h"
//constant define
#define MAX_NUM_GATTC_CB    (10)
#define MAX_NUM_GAP_CB      (10)
#define QUEUE_NUM_ELEMENTS  (32)
// MODULE TYPE

ble_evt_cb_t gattc_cb_table[MAX_NUM_GATTC_CB];
ble_evt_cb_t gap_cb_table[MAX_NUM_GAP_CB];

// MODULE VARIABLE 
static struct
{
  ble_state_t current_state;
  struct 
  {
    uint16_t interface;
    uint16_t conn_id;
  } gattc;
  uint8_t remote_addr[6];
  uint8_t num_gattc_cb_registered;
  uint8_t num_gap_cb_registered;
  // StaticTask_t thread_handle;
  // uint8_t      thread_stack[8192];
  // QueueHandle_t evt_queue;
} ble;

// PRIVATE FUNCTION PROTOTYPE
static void ble_stack_init();
static void ble_sec_init();
static void ble_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
static void ble_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
static void ble_gattc_evt_dispatch(esp_gattc_cb_event_t event, esp_ble_gattc_cb_param_t *param);
static void ble_gap_evt_dispatch(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
// PUBLIC FUNCTIONS
void ble_init()
{
  // init some ble module
  ble_stack_init();
  // register callback
  esp_ble_gap_register_callback(ble_gap_cb);
  esp_ble_gattc_register_callback(ble_gattc_cb);
  esp_ble_gattc_app_register(CONFIG_BLE_GATTC_APP_ID);
  // Init ble App module
  ble_sec_init();
  ble_svc_dis_init(); // Init before other service
  ble_adv_init();
  ble_cts_init();

  esp_ble_gatt_set_local_mtu(500);
  //ble_adv_start();
  // Create thread for handle event from stack
  // xTaskCreateStatic(ble_thread_entry,
  //                   "ble_thread",
  //                   sizeof(ble.thread_stack),
  //                   NULL,
  //                   2,
  //                   ble.thread_stack,
  //                   &ble.thread_handle);
  // // Create Queue for this 
  // ble.evt_queue = xQueueCreate(QUEUE_NUM_ELEMENTS, sizeof()
}

uint16_t ble_get_gattc_if()
{
  return ble.gattc.interface;
}

uint16_t ble_get_gattc_conn_id()
{
  return ble.gattc.conn_id;

}

uint8_t * ble_get_gattc_remote_bda()
{
  return ble.remote_addr;
}
void ble_gattc_register_cb(ble_evt_cb_t cb)
{
  if(ble.num_gattc_cb_registered >= MAX_NUM_GATTC_CB) return;
  gattc_cb_table[ble.num_gattc_cb_registered] = cb;
  ble.num_gattc_cb_registered++;
}

void ble_gap_register_cb(ble_evt_cb_t cb)
{
  if(ble.num_gap_cb_registered >= MAX_NUM_GAP_CB) return;
  gap_cb_table[ble.num_gap_cb_registered] = cb;
  ble.num_gap_cb_registered++;
}

// PRIVATE FUNCTIONS
static void ble_sec_init()
{
  /* set the security iocap & auth_req & key size & init key response key parameters to the stack*/
  esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_MITM_BOND;     //bonding with peer device after authentication
  esp_ble_io_cap_t iocap = ESP_IO_CAP_NONE;           //set the IO capability to No output No input
  //uint8_t key_size = 16;      //the key size should be 7~16 bytes
  uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK | ESP_BLE_CSR_KEY_MASK;
  uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK | ESP_BLE_CSR_KEY_MASK;
  //set static passkey
  //uint32_t passkey = 123456;
  uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;
  uint8_t oob_support = ESP_BLE_OOB_DISABLE;
  //esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
  //esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &auth_option, sizeof(uint8_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_OOB_SUPPORT, &oob_support, sizeof(uint8_t));
  /* If your BLE device acts as a Slave, the init_key means you hope which types of key of the master should distribute to you,
  and the response key means which key you can distribute to the master;
  If your BLE device acts as a master, the response key means you hope which types of key of the slave should distribute to you,
  and the init key means which key you can distribute to the slave. */
  esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));
}

static void ble_stack_init()
{
  esp_err_t ret;
  // Initialize NVS.
  ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) 
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);

  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  esp_bt_controller_init(&bt_cfg);
  esp_bt_controller_enable(ESP_BT_MODE_BLE);
  esp_bluedroid_init();
  esp_bluedroid_enable();
}

static void ble_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
  switch(event)
  {
    case ESP_GATTC_REG_EVT:
      ble.gattc.interface = gattc_if;
      break;
    case ESP_GATTC_OPEN_EVT:
      ble.gattc.conn_id  = param->open.conn_id;
      esp_ble_set_encryption(param->open.remote_bda, ESP_BLE_SEC_ENCRYPT_MITM);
      esp_ble_gattc_send_mtu_req(gattc_if, param->open.conn_id);
      esp_ble_gap_set_pkt_data_len(ble.remote_addr, 250);
      break;
    case ESP_GATTC_CONNECT_EVT:
      // this to make device bond after connecting
      ble.current_state = BLE_STATE_CONNECTED;
      // store the remote address
      for(int i = 0; i < 6; i++) ble.remote_addr[i] = param->connect.remote_bda[i];
      // create gattc virtual connection
      esp_ble_gattc_open(ble.gattc.interface, param->connect.remote_bda, BLE_ADDR_TYPE_RANDOM, true);
      break;
    default:
      break;
  }
  // dispatch evt to upper layer
  ble_gattc_evt_dispatch(event, param);
}

static void ble_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
  switch(event)
  {
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
      ble.current_state = BLE_STATE_ADVERTISING;
      break;
    case ESP_GAP_BLE_AUTH_CMPL_EVT:
      ble.current_state = BLE_STATE_PAIRED;
      ESP_LOGI("BLE","Authentication complete\r\n");
      break;
    default:
      break;
  }
  // dispatch evt to upper layer
  ble_gap_evt_dispatch(event, param);
}

static void ble_gattc_evt_dispatch(esp_gattc_cb_event_t event, esp_ble_gattc_cb_param_t *param)
{
  //printf("ble gattc dispatch:%d num cb\r\n", ble.num_gattc_cb_registered);
  ble_evt_t gattc_evt = 
  {
    .gattc_evt = event,
    .gattc_param = param
  };
  for(uint8_t i = 0; i < ble.num_gattc_cb_registered; i++)
  {
    if(gattc_cb_table[i]) gattc_cb_table[i](gattc_evt);
  }
}

static void ble_gap_evt_dispatch(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
  //printf("ble gap dispatch evt:%d \r\n", event);
  ble_evt_t gap_evt = 
  {
    .gap_evt = event,
    .gap_param = param
  };
  for(uint8_t i = 0; i < ble.num_gap_cb_registered; i++)
  {
    if(gap_cb_table[i]) gap_cb_table[i](gap_evt);
  }
}