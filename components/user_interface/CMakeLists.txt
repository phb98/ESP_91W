
file(GLOB_RECURSE UI_MAIN_SOURCE "*.c")
file(GLOB_RECURSE UI_NODE_SOURCE "nodes/*.c")

list(APPEND UI_SOURCE ${UI_MAIN_SOURCE})
list(APPEND UI_SOURCE ${UI_NODE_SOURCE})

set(UI_INCLUDE_DIRS "."
                    "../lvgl"
                    "../lvgl_font"
                    "../SSD1306"
                    "../../config"
                    "../../etc"
                    "../ble"
                    "./nodes")
                    
idf_component_register(SRCS          ${UI_SOURCE}
                       INCLUDE_DIRS  ${UI_INCLUDE_DIRS}
                       REQUIRES      "nvs_flash"
                                     " bt"
                                     "lvgl_font"
                                     "lvgl")

                                     
