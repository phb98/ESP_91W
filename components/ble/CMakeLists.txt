file(GLOB_RECURSE BLE_SOURCE "ble*.c"
                             "gattc/ble*.c")
idf_component_register(SRCS ${BLE_SOURCE}
                      INCLUDE_DIRS  "."
                                     "../lvgl"
                                     "../SSD1306"
                                     "../../config"
                                     "../../etc"
                                     "../user_interface"
                      REQUIRES      "nvs_flash"
                                    "bt")