#include "ble_gatts_svc.h"
#include "esp_gatts_api.h"
#include "esp_log.h"
#include "ble.h"
#include <string.h>
/*************************************************************************************/
/*                                  CONSTANT DEFINE                                  */
/*************************************************************************************/
#define MAX_NUM_SVC_REGISTER    10
#define BLE_GATTS_SVC_LOGI(...) ESP_LOGI("BLE_GATTS_SVC", __VA_ARGS__)
/*************************************************************************************/
/*                                    MODULE TYPE                                    */
/*************************************************************************************/

/*************************************************************************************/
/*                                  MODULE VARIABLE                                  */
/*************************************************************************************/
static struct{
  ble_gatts_svc_t * p_registering_svc;
  struct{
  ble_gatts_svc_t * p_tail;
  ble_gatts_svc_t * p_head;
  } ll_svc;
} ble_gatts_svc;
/*************************************************************************************/
/*                            PRIVATE FUNCTION PROTOTYPE                             */
/*************************************************************************************/
static void ble_gatts_cb(ble_evt_t evt);
static void ble_gatts_add_svc_db(ble_gatts_svc_t * p_svc);
/*************************************************************************************/
/*                                  PUBLIC FUNCTION                                  */
/*************************************************************************************/
void ble_gatts_svc_init()
{
  ble_gatts_register_cb(ble_gatts_cb);
}

void ble_gatts_svc_register(ble_gatts_svc_t * register_svc)
{
  if(!register_svc || !register_svc->p_svc_db || !register_svc->p_handles) return;
  BLE_GATTS_SVC_LOGI("Register Service:%s", register_svc->svc_name);
  // Add this svc to our linked list
  if(!ble_gatts_svc.ll_svc.p_tail) ble_gatts_svc.ll_svc.p_tail = register_svc;
  if(ble_gatts_svc.ll_svc.p_head) ble_gatts_svc.ll_svc.p_head->p_next_svc = register_svc;
  ble_gatts_svc.ll_svc.p_head = register_svc;
  // IF there is no adding service, start adding now
  if(!ble_gatts_svc.p_registering_svc)
  {
    ble_gatts_svc.p_registering_svc = register_svc;
    ble_gatts_add_svc_db(register_svc);
  }
}

/*************************************************************************************/
/*                                 PRIVATE FUNCTION                                  */
/*************************************************************************************/
static void ble_gatts_add_svc_db(ble_gatts_svc_t * p_svc)
{
  static uint16_t num_registered_svc = 0;
  if(!p_svc || !p_svc->p_svc_db)
  {
    esp_ble_gatts_create_attr_tab(p_svc->p_svc_db,
                                  ble_get_gatts_if(), 
                                  p_svc->num_attr, 
                                  num_registered_svc++);
    p_svc->svc_id = num_registered_svc;
  }
}

static void ble_gatts_cb(ble_evt_t evt)
{
  esp_ble_gatts_cb_param_t * param = evt.gatts_param;
  esp_gatts_cb_event_t event = evt.gatts_evt;
  switch(event)
  {
    case ESP_GATTS_CREAT_ATTR_TAB_EVT:
    {
      if(param->add_attr_tab.status != ESP_GATT_OK)
      {
        BLE_GATTS_SVC_LOGI("Add Service %s failed", ble_gatts_svc.p_registering_svc->svc_name);
        // Add another Svc
        if(ble_gatts_svc.p_registering_svc->p_next_svc)
        {
          ble_gatts_add_svc_db(ble_gatts_svc.p_registering_svc->p_next_svc);
          ble_gatts_svc.p_registering_svc = ble_gatts_svc.p_registering_svc->p_next_svc;
        }
        else
        {
          ble_gatts_svc.p_registering_svc = NULL; // No more service to add
        }
        break;
      }
      BLE_GATTS_SVC_LOGI("Add Service %s Success", ble_gatts_svc.p_registering_svc->svc_name);
      memcpy(ble_gatts_svc.p_registering_svc->p_handles, param->add_attr_tab.handles, sizeof(uint16_t) * ble_gatts_svc.p_registering_svc->num_attr);
      esp_ble_gatts_start_service(ble_gatts_svc.p_registering_svc->p_handles[0]); // Start service
      break;
    }
    case ESP_GATTS_START_EVT:
    {
      if(param->start.status != ESP_GATT_OK) BLE_GATTS_SVC_LOGI("Start Service %s failed", ble_gatts_svc.p_registering_svc->svc_name);
      else BLE_GATTS_SVC_LOGI("Start Service %s Success", ble_gatts_svc.p_registering_svc->svc_name);
      // Add another Svc
      if(ble_gatts_svc.p_registering_svc->p_next_svc)
      {
        ble_gatts_add_svc_db(ble_gatts_svc.p_registering_svc->p_next_svc);
        ble_gatts_svc.p_registering_svc = ble_gatts_svc.p_registering_svc->p_next_svc;
      }
      else
      {
        ble_gatts_svc.p_registering_svc = NULL; // No more service to add
      }
      break;
    }
    default:
      break;
  }
}
