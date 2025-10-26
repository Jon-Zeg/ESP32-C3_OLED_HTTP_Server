#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include "esp_err.h"

// OLED SSD1306 0.42" definitions for ESP32-C3
#define OLED_I2C_ADDRESS    0x3C
#define OLED_WIDTH          128  // Configuration resolution
#define OLED_HEIGHT         64   // Configuration resolution
#define OLED_REAL_WIDTH     72   // Effective real resolution
#define OLED_REAL_HEIGHT    40   // Effective real resolution
#define OLED_OFFSET_X       28   // Horizontal offset for correct rendering (reduced to move left)
#define OLED_OFFSET_Y       24   // Vertical offset for correct rendering (increased to move down)
#define OLED_I2C_NUM        I2C_NUM_0
#define OLED_SDA_GPIO       5    // GPIO5 for SDA according to specifications
#define OLED_SCL_GPIO       6    // GPIO6 for SCL according to specifications

// Comandos del SSD1306
#define SSD1306_SET_CONTRAST            0x81
#define SSD1306_DISPLAY_ALL_ON_RESUME   0xA4
#define SSD1306_DISPLAY_ALL_ON          0xA5
#define SSD1306_NORMAL_DISPLAY          0xA6
#define SSD1306_INVERT_DISPLAY          0xA7
#define SSD1306_DISPLAY_OFF             0xAE
#define SSD1306_DISPLAY_ON              0xAF
#define SSD1306_SET_DISPLAY_OFFSET      0xD3
#define SSD1306_SET_COM_PINS            0xDA
#define SSD1306_SET_VCOM_DETECT         0xDB
#define SSD1306_SET_DISPLAY_CLOCK_DIV   0xD5
#define SSD1306_SET_PRECHARGE           0xD9
#define SSD1306_SET_MULTIPLEX           0xA8
#define SSD1306_SET_LOW_COLUMN          0x00
#define SSD1306_SET_HIGH_COLUMN         0x10
#define SSD1306_SET_START_LINE          0x40
#define SSD1306_MEMORY_ADDR_MODE        0x20
#define SSD1306_SET_COLUMN_RANGE        0x21
#define SSD1306_SET_PAGE_RANGE          0x22
#define SSD1306_COM_SCAN_INC            0xC0
#define SSD1306_COM_SCAN_DEC            0xC8
#define SSD1306_SEG_REMAP               0xA0
#define SSD1306_CHARGE_PUMP             0x8D
#define SSD1306_EXTERNAL_VCC            0x01
#define SSD1306_SWITCH_CAP_VCC          0x02

// Public functions
esp_err_t oled_init(void);
esp_err_t oled_clear(void);
esp_err_t oled_display_text(const char* text, int x, int y);
esp_err_t oled_display_text_scaled(const char* text, int x, int y);
esp_err_t oled_display_dataola(void);
esp_err_t oled_update_display(void);
esp_err_t oled_scan_i2c_devices(void);
esp_err_t oled_test_pattern(void);

#endif // OLED_DISPLAY_H