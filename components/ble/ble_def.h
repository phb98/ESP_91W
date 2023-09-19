/************************************************************************************************************
Module:       ble_def.h

************************************************************************************************************/

#ifndef _BLE_DEF_H
#define _BLE_DEF_H

//###########################################################################################################
// #INCLUDES
//###########################################################################################################
#include "esp_bt_defs.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>

//###########################################################################################################
// MACROS
#define BLE_UUID_DECLARE_16(UUID) {.len = ESP_UUID_LEN_16, .uuid.uuid16 = UUID}
#define BLE_UUID_DECLARE_32(UUID) {.len = ESP_UUID_LEN_32, .uuid.uuid32 = UUID}
#define BLE_UUID_DECLARE_128(p_uuid) {.len = ESP_UUID_LEN_128, .uuid.uuid128 = p_uuid}

//###########################################################################################################
// DEFINED CONSTANTS
//###########################################################################################################
#define    BLE_INVALID_CHAR_HDL           (0x0)

#define    BLE_CHAR_PROP_BIT_BROADCAST    ((ble_char_prop_t)(1 << 0))       /* 0x01 */ 
#define    BLE_CHAR_PROP_BIT_READ         ((ble_char_prop_t)(1 << 1))       /* 0x02 */ 
#define    BLE_CHAR_PROP_BIT_WRITE_NR     ((ble_char_prop_t)(1 << 2))       /* 0x04 */ 
#define    BLE_CHAR_PROP_BIT_WRITE        ((ble_char_prop_t)(1 << 3))       /* 0x08 */ 
#define    BLE_CHAR_PROP_BIT_NOTIFY       ((ble_char_prop_t)(1 << 4))       /* 0x10 */ 
#define    BLE_CHAR_PROP_BIT_INDICATE     ((ble_char_prop_t)(1 << 5))       /* 0x20 */ 
#define    BLE_CHAR_PROP_BIT_AUTH         ((ble_char_prop_t)(1 << 6))       /* 0x40 */ 
#define    BLE_CHAR_PROP_BIT_EXT_PROP     ((ble_char_prop_t)(1 << 7))       /* 0x80 */ 
//###########################################################################################################
// DEFINED TYPES
//###########################################################################################################
typedef uint16_t      ble_char_hdl_t;
typedef uint8_t       ble_char_prop_t;
typedef esp_bt_uuid_t ble_uuid_t;
typedef struct
{
  ble_char_hdl_t  hdl;
  ble_uuid_t      uuid;
  ble_char_prop_t prop;
}ble_char_t;
typedef struct
{
  const char * const svc_name;
  const ble_uuid_t   svc_uuid;
  ble_char_hdl_t     start_hdl;
  ble_char_hdl_t      end_hdl;
  ble_char_t * const char_list;
  uint16_t num_char;
} ble_svc_t;

//###########################################################################################################
// DEFINED MACROS
//###########################################################################################################



//###########################################################################################################
// FUNCTION PROTOTYPES
//###########################################################################################################

//###########################################################################################################
// END OF ble_def.h
//###########################################################################################################
#endif
