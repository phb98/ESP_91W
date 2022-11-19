idf_component_register(SRCS "display.c"
                       INCLUDE_DIRS  "."
                                     "../lvgl"
                                     "../SSD1306"
                                     "../../config"
                        REQUIRES "SSD1306")
