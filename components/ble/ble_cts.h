#ifndef _BLE_CTS_H
#define _BLE_CTS_H
#include <stdint.h>
typedef struct
{
  uint16_t year;
  uint8_t  mon;
  uint8_t  day;
  uint8_t  hour;
  uint8_t  min;
  uint8_t  sec;
  uint8_t  dow;
} ble_cts_time_t;
void ble_cts_init();
ble_cts_time_t ble_cts_get_time();
#endif