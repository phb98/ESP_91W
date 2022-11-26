#ifndef _BLE_SVC_DIS_H
#define _BLE_SVC_DIS_H
#include "ble.h"

typedef enum
{
  BLE_SVC_DIS_SUCCESS,
  BLE_SVC_DIS_FAIL,
  BLE_SVC_DIS_DISCONNECTED
} ble_svc_dis_status_t;
typedef void (*ble_svc_dis_cb_t)(ble_svc_dis_status_t status);
typedef struct
{
  ble_svc_t * request_svc;
  ble_svc_dis_cb_t cb;
} ble_svc_dis_t;

void ble_svc_dis_init();
void ble_svc_dis_request(ble_svc_dis_t svc);
void ble_svc_dis_reset(ble_svc_t * svc);
#endif