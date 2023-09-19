/************************************************************************************************************
Module:       ble_disc.c

************************************************************************************************************/

//###########################################################################################################
//      #INCLUDES
//###########################################################################################################
#define MODULE_NAME BLE_DISC
#include "ble.h"
#include "ble_disc.h"
#include "ble_log.h"
#include "ble_def.h"
//###########################################################################################################
//      CONSTANT #DEFINES
//###########################################################################################################



//###########################################################################################################
//      MACROS
//###########################################################################################################
#define PARAM_NULL_CHECK(p) if(!(p)) {BLE_ERROR("Invalid parameter NULL"); return;}


//###########################################################################################################
//      MODULE TYPES
//###########################################################################################################
typedef struct
{
  bool is_discovered;
  ble_svc_t * p_svc;
  ble_disc_cb_t * p_cb;
} ble_disc_svc_t;

//###########################################################################################################
//      CONSTANTS
//###########################################################################################################



//###########################################################################################################
//      MODULE LEVEL VARIABLES
//###########################################################################################################
struct 
{
  ble_disc_svc_t * p_discovering_svc;
} ble_disc;


//###########################################################################################################
//      PRIVATE FUNCTION PROTOTYPES
//###########################################################################################################
static void ble_disc_gattc_cb(ble_evt_t evt);


//###########################################################################################################
//      PUBLIC FUNCTIONS
//###########################################################################################################
void ble_disc_init()
{
  BLE_INFO("BLE DISC INIT");
  ble_gattc_register_cb(ble_disc_gattc_cb);
}

void ble_disc_request(ble_svc_t * p_svc, ble_disc_cb_t * p_cb)
{
  if(!p_svc || !p_cb)
  {
    BLE_ERROR("Invalid parameter");
    return;
  }
  BLE_INFO("Requesting discovery for service: %s", p_svc->svc_name);
  if(ble_disc.discovering_svc.p_svc)
  {
    // Todo - add to linked list of services to discover
    BLE_ERROR("Already discovering a service");
    BLE_WARN("TODO: Add to linked list of services to discover");
    return;
  }
  else
  {
    ble_disc_svc_t * p_disc_svc = ble_disc_svc_new(p_svc, p_cb);
    
    ble_disc.discovering_svc.p_svc = p_svc;
    ble_disc.discovering_svc.p_cb  = p_cb;
    ble_disc.discovering_svc.is_discovered = false;
    ble_disc_svc_reset(p_svc);
    
    if(ble_disc_start(&ble_disc.discovering_svc) == false)
    {
      // Failed to start discovery
      ble_disc.discovering_svc.p_svc = NULL;
      ble_disc.discovering_svc.p_cb  = NULL;
      ble_disc.discovering_svc.is_discovered = false;
    }
  }
}
//###########################################################################################################
//      PRIVATE FUNCTIONS
//###########################################################################################################
static ble_disc_svc_t * ble_disc_svc_new(ble_svc_t * const p_svc, ble_disc_cb_t * const p_cb)
{
  BLE_DEBUG("Malloc ble_disc_svc_t");
  ble_disc_svc_t * p_disc_svc = malloc(sizeof(ble_disc_svc_t));
  if(!p_disc_svc)
  {
    BLE_ERROR("Failed to allocate memory for ble_disc_svc_t");
    return NULL;
  }
  p_disc_svc->p_svc = p_svc;
  p_disc_svc->p_cb  = p_cb;
  p_disc_svc->is_discovered = false;
  return p_disc_svc;
}

static void ble_disc_svc_reset(ble_svc_t * p_svc)
{
  PARAM_NULL_CHECK(p_svc);
  p_svc->start_hdl = p_svc->end_hdl = BLE_INVALID_CHAR_HDL;
}
//###########################################################################################################
//      BLE_DISC FUNCTIONS
//###########################################################################################################
static void ble_disc_gattc_cb(ble_evt_t evt)
{
  esp_gattc_cb_event_t      event = evt.gattc_evt; 
  esp_ble_gattc_cb_param_t *param = evt.gattc_param;
  ble_disc_svc_t * p_discovering_svc = ble_disc_get_discovering_svc();
  PARAM_NULL_CHECK(param);
  if(!(p_discovering_svc->p_svc))
  {
    BLE_DEBUG("Not discovering a service, ignoring event");
    return;
  }

  switch(event)
  {
    case ESP_GATTC_SEARCH_RES_EVT:
      break;
    case ESP_GATTC_SEARCH_CMPL_EVT:
      break;
    default:
      break;
  }
}

/**
 * @brief Start discovery of a service
 * @return true if discovery started success, false otherwise
 */
*/
static bool ble_disc_start(ble_disc_svc_t * p_disc_svc)
{
  PARAM_NULL_CHECK(p_disc_svc);
  esp_err_t err = esp_ble_gattc_search_service(ble_get_gattc_if(), ble_get_gattc_conn_id(), &(p_disc_svc->p_svc->svc_uuid));
  if(err != ESP_OK)
  {
    BLE_ERROR("Failed to start discovery of service: %d", err);
    return false;
  }
  return true;
}

static ble_disc_svc_t * ble_disc_get_discovering_svc()
{
  return &ble_disc.discovering_svc;
}
//###########################################################################################################
//      INTERRUPTS
//###########################################################################################################



//###########################################################################################################
//      END OF ble_disc.c
//###########################################################################################################

