file(GLOB_RECURSE lvgl_font_source *.c)

idf_component_register(SRCS ${lvgl_font_source}
                       INCLUDE_DIRS  "."
                                     "../lvgl"
                      )
