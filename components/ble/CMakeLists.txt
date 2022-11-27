file(GLOB_RECURSE BLE_SOURCE "*.c")
idf_component_register(SRCS ${BLE_SOURCE}
                      INCLUDE_DIRS  "."
                                     "../lvgl"
                                     "../SSD1306"
                                     "../../config"
                                     "../../etc"
                      REQUIRES      "nvs_flash"
                                    "bt")