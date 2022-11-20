#ifndef _SSD1306_CONF_H
#define _SSD1306_CONF_H
#define CONFIG_OLED_I2C_PORT                (0)
#define CONFIG_OLED_I2C_FREQ                (400000UL)
#define CONFIG_OLED_I2C_SDA                 (5)
#define CONFIG_OLED_I2C_SCL                 (6)

#define OLED_I2C_ADDRESS                    (0x3C)
#define OLED_WIDTH                          (64)
#define OLED_HEIGHT                         (48)
#define OLED_COLUMNS                        OLED_WIDTH
#define OLED_CONTRAST                       0x3F
#endif