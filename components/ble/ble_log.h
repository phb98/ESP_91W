#ifndef BLE_LOG_H
#define BLE_LOG_H
#include "esp_log.h"
#define XSTR(s) ___STR(s)
#define ___STR(s) #s
#define STR(a) a

#ifndef MODULE_NAME
#define __MODULE__FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#define MODULE_NAME_STR __MODULE__FILENAME__
#else
#define MODULE_NAME_STR XSTR(MODULE_NAME)
#endif
#define BLE_ERROR(fmt, ...)   ESP_LOGE(MODULE_NAME_STR, "[%s]:" fmt, __func__, ##__VA_ARGS__)
#define BLE_WARN(fmt,...)     ESP_LOGW(MODULE_NAME_STR, "[%s]:" fmt, __func__, ##__VA_ARGS__)
#define BLE_INFO(fmt,...)     ESP_LOGI(MODULE_NAME_STR, "[%s]:" fmt, __func__, ##__VA_ARGS__)  
#define BLE_DEBUG(fmt,...)    ESP_LOGD(MODULE_NAME_STR, "[%s]:" fmt, __func__, ##__VA_ARGS__)
#define BLE_VERBOSE(fmt,...)  ESP_LOGV(MODULE_NAME_STR, "[%s]:" fmt, __func__, ##__VA_ARGS__)
#endif