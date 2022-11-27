#include "display.h"
#include "lvgl.h"
#include "SSD1306.h"
#include "SSD1306_conf.h"
#include "thread_config.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
lv_disp_draw_buf_t disp_buf;
static lv_color_t  disp_buf_draw_A[OLED_WIDTH*OLED_HEIGHT];
static lv_color_t  disp_buf_draw_B[OLED_WIDTH*OLED_HEIGHT];
lv_disp_drv_t disp_drv;                 /*A variable to hold the drivers. Can be local variable*/
lv_disp_t * disp;
static uint8_t display_stack[CONFIG_THREAD_DISPLAY_STACK_SIZE];
static StaticTask_t display_thread_handle;

// PRIVATE FUNCTION PROTOTYPE
static void display_thread(void * param);
static void display_lvgl_init();
// PUBLIC FUNCTION
void display_init()
{
  // init lvgl and display
  display_lvgl_init();
  ssd1306_init();
  // Init driver
  lv_disp_draw_buf_init(&disp_buf, disp_buf_draw_A, disp_buf_draw_B, OLED_WIDTH*OLED_HEIGHT);
  lv_disp_drv_init(&disp_drv);            /*Basic initialization*/
  disp_drv.draw_buf = &disp_buf;            /*Set an initialized buffer*/
  disp_drv.hor_res  = OLED_WIDTH;
  disp_drv.ver_res  = OLED_HEIGHT;
  disp_drv.physical_hor_res = OLED_WIDTH;
  disp_drv.physical_ver_res = OLED_HEIGHT;
  disp_drv.antialiasing = 0;
  disp_drv.full_refresh = true;
  disp_drv.flush_cb = ssd1306_flush;        /*Set a flush callback to draw to the display*/
  disp_drv.rounder_cb = ssd1306_rounder;
  disp = lv_disp_drv_register(&disp_drv); /*Register the driver and save the created display objects*/
  // Create thread to run LVGL task
  xTaskCreateStatic(display_thread,
                    CONFIG_THREAD_DISPLAY_NAME,
                    CONFIG_THREAD_DISPLAY_STACK_SIZE,
                    NULL,
                    CONFIG_THREAD_DISPLAY_PRIORITY,
                    display_stack,
                    &display_thread_handle);
}

static void display_thread(void * param)
{
  while(1)
  {
    vTaskDelay(10/portTICK_PERIOD_MS);
    lv_tick_inc(10);
    lv_timer_handler();
  }
}

static void display_lvgl_init()
{
  // we may init more than 1 thing here, so wrap all in this function
  lv_init();
}