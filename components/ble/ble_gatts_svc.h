#ifndef _BLE_GATTS_SVC_H
#define _BLE_GATTS_SVC_H
#include "esp_gatt_defs.h"
#include "std_type.h"
/*************************************************************************************/
/*                                  CONSTANT DEFINE                                  */
/*************************************************************************************/

/*************************************************************************************/
/*                                    MODULE TYPE                                    */
/*************************************************************************************/
typedef struct ble_gatts_svc_t
{
  struct ble_gatts_svc_t * p_next_svc;
  const char * svc_name;
  esp_gatts_attr_db_t * p_svc_db;
  uint16_t num_attr;
  uint16_t * p_handles; // this will be overwrite by bluetooth stack
  uint16_t svc_id;
} ble_gatts_svc_t;

/**
 * The registering service should be static
*/
void ble_gatts_svc_register(ble_gatts_svc_t * register_svc);
/*************************************************************************************/
/*                                  PUBLIC FUNCTION                                  */
/*************************************************************************************/
#endif