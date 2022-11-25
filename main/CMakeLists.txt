include("../etc/common.cmake")
idf_component_register(SRCS "esp_watch.c"
                       INCLUDE_DIRS  "."
                                     "../etc"
                                     "../components/display"
                                     "../components/lvgl"
                       REQUIRES      "lvgl"
                                     "ble")
                                     
