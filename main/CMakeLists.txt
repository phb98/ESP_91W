include("../etc/common.cmake")
set(COMPONENT_PATH "../components")
idf_component_register(SRCS "esp_watch.c"
                       INCLUDE_DIRS  "."
                                     "../etc"
                                     "${COMPONENT_PATH}/display"
                                     "${COMPONENT_PATH}/lvgl"
                                     "${COMPONENT_PATH}/user_interface"
                                     "${COMPONENT_PATH}/lvgl_font"
                       REQUIRES      "lvgl"
                                     "ble")
                                     
