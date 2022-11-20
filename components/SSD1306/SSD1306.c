/**
 * @file ssd1306.c
 *
 * Code from https://github.com/yanbe/ssd1306-esp-idf-i2c.git is used as a starting point,
 * in addition to code from https://github.com/espressif/esp-iot-solution.
 *
 * Definitions are borrowed from:
 * http://robotcantalk.blogspot.com/2015/03/interfacing-arduino-with-ssd1306-driven.html
 *
 * For LVGL the forum has been used, in particular: https://blog.littlevgl.com/2019-05-06/oled
 */

/*********************
 *      INCLUDES
 *********************/
#include "assert.h"
#include "SSD1306.h"
#include "driver/i2c.h"
#include "SSD1306_reg.h"
#include "SSD1306_conf.h"
/*********************
 *      DEFINES
 *********************/

// Control byte
#define OLED_CONTROL_BYTE_CMD_STREAM        0x00
#define OLED_CONTROL_BYTE_DATA_STREAM       0x40

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static uint8_t send_pixels(const uint8_t *color_buffer, size_t buffer_len);
static uint8_t send_cmd(const uint8_t *cmd, uint16_t cmd_len);
static uint8_t i2c_write(const uint8_t cmd, uint8_t *buffer, uint32_t buffer_len);
static void    i2c_init();
/**********************
 *  STATIC VARIABLES
 **********************/
static uint8_t frame_buffer[OLED_WIDTH * ((OLED_HEIGHT + 7) / 8)];
/**********************
 *      MACROS
 **********************/

#define BIT_SET(a,b) ((a) |= (1U<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1U<<(b)))

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void ssd1306_init(void)
{
  i2c_init();
  // Init sequence
  // Base on Adafruit SSD1306 driver
  static const uint8_t init1[] = {SSD1306_DISPLAYOFF,         // 0xAE
                                  SSD1306_SETDISPLAYCLOCKDIV, // 0xD5
                                  0x80, // the suggested ratio 0x80
                                  SSD1306_SETMULTIPLEX, 
                                  OLED_HEIGHT - 1};
  send_cmd(init1, sizeof(init1));

  static const uint8_t init2[] = {SSD1306_SETDISPLAYOFFSET, // 0xD3
                                  0x0,                      // no offset
                                  SSD1306_SETSTARTLINE | 0x0, // line #0
                                  SSD1306_CHARGEPUMP,
                                  0x14};        
  send_cmd(init2, sizeof(init2));

  static const uint8_t init3[] = {SSD1306_MEMORYMODE, // 0x20
                                  0x00, // 0x0 act like ks0108
                                  SSD1306_SEGREMAP | 0x1,
                                  SSD1306_COMSCANDEC};
  send_cmd(init3, sizeof(init3));

  static const uint8_t init4[] = {SSD1306_SETCOMPINS, // 0x20
                                  0x12, // 0x0 act like ks0108
                                  SSD1306_SETCONTRAST,
                                  OLED_CONTRAST};
  send_cmd(init4, sizeof(init4));

  static const uint8_t init5[] = {SSD1306_SETVCOMDETECT, // 0xDB
                                  0x40,
                                  SSD1306_DISPLAYALLON_RESUME, // 0xA4
                                  SSD1306_NORMALDISPLAY,       // 0xA6
                                  SSD1306_DEACTIVATE_SCROLL,
                                  SSD1306_DISPLAYON}; // Main screen turn on
  send_cmd(init5, sizeof(init5));
}

//LVGL DROP SUPPORT FOR THIS FUNCTION
// void ssd1306_set_px_cb(lv_disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
//         lv_color_t color, lv_opa_t opa)
// {
//     uint16_t byte_index = x + (( y>>3 ) * buf_w);
//     uint8_t  bit_index  = y & 0x7;

//     if ((color.full == 0) && (LV_OPA_TRANSP != opa)) {
//         BIT_SET(buf[byte_index], bit_index);
//     } else {
//         BIT_CLEAR(buf[byte_index], bit_index);
//     }
// }

void ssd1306_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
  #if (OLED_WIDTH == 64 && OLED_HEIGHT == 48)
  #define WIDTH_OFFSET 32
  #else
  #define WIDTH_OFFSET 0
  #endif
  static const uint8_t dlist1[] = {
                                    SSD1306_PAGEADDR,
                                    0,                      // Page start address
                                    0xFF,                   // Page end (not really, but works here)
                                    SSD1306_COLUMNADDR, WIDTH_OFFSET, WIDTH_OFFSET + OLED_WIDTH - 1}; // Column start address
  send_cmd(dlist1, sizeof(dlist1));
  
  //Prepare frame buffer
  //This is slow, but lvgl drop support for pixel_cb, so we have to do it ourself
  memset(frame_buffer, 0x0, sizeof(frame_buffer));
  for(int y = area->y1; y <= area->y2; y++)
  {
    for(int x = area->x1; x <= area->x2; x++)
    {
      if(color_p->full > 0) frame_buffer[x + (y / 8) * OLED_WIDTH] |= (1 << (y & 7));
      color_p++;
    }
  }
  send_pixels(frame_buffer, sizeof(frame_buffer));
  lv_disp_flush_ready(disp_drv);
}

void ssd1306_rounder(lv_disp_drv_t * disp_drv, lv_area_t *area)
{
  (void)disp_drv;
  area->x1 = 0;
  area->y1 = 0;
  // always froce full screen refresh, can do partial update later
  area->x2 = OLED_WIDTH - 1;
  area->y2 = OLED_HEIGHT - 1;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/
static uint8_t send_pixels(const uint8_t *color_buffer, size_t buffer_len)
{
  return i2c_write(OLED_CONTROL_BYTE_DATA_STREAM, (uint8_t*)color_buffer, buffer_len);
}
static uint8_t send_cmd(const uint8_t *cmd, uint16_t cmd_len)
{
  return i2c_write(OLED_CONTROL_BYTE_CMD_STREAM, (uint8_t*)cmd, cmd_len);
}

static void i2c_init()
{
  i2c_config_t conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = CONFIG_OLED_I2C_SDA, 
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_io_num = CONFIG_OLED_I2C_SCL,         
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
      .master.clk_speed = CONFIG_OLED_I2C_FREQ,  // select frequency specific to your project
      .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL,  // you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here
  };
  i2c_driver_install(CONFIG_OLED_I2C_PORT, I2C_MODE_MASTER, 0, 0, ESP_INTR_FLAG_LOWMED);
  i2c_param_config(CONFIG_OLED_I2C_PORT, &conf);
  //test interface
  static uint8_t test_buf[1];
  esp_err_t ret = i2c_master_write_to_device(CONFIG_OLED_I2C_PORT, OLED_I2C_ADDRESS, test_buf, 0x1, 0xFF);
  if(ret != ESP_OK)
  {
    printf("SSD1306 Init Fail:%d\r\n", ret);
    assert(0);
  }
}


uint8_t i2c_write(const uint8_t cmd, uint8_t *buffer, uint32_t buffer_len)
{
  i2c_cmd_handle_t cmd_link = i2c_cmd_link_create();
  i2c_master_start(cmd_link);
  //i2c_send_address(cmd_link, OLED_I2C_ADDRESS, I2C_MASTER_WRITE);
  i2c_master_write_byte(cmd_link, (OLED_I2C_ADDRESS << 1) | 0, true);
  i2c_master_write_byte(cmd_link, cmd, true);
  i2c_master_write(cmd_link, buffer, buffer_len, true);
  i2c_master_stop(cmd_link);
  esp_err_t ret = i2c_master_cmd_begin(CONFIG_OLED_I2C_PORT, cmd_link, 0xFFFFFFF);
  i2c_cmd_link_delete(cmd_link);
  return ret;
}
