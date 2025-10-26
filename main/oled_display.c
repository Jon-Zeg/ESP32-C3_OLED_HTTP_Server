#include "oled_display.h"
#include "driver/i2c.h"
#include "esp_log.h"
#i// Private fun// Private function to send data to OLED
static esp_err_t oled_write_data(const uint8_t* data, size_t len) {ion to send command to OLED
static esp_err_t oled_write_command(uint8_t cmd) {lude <string.h>

static const char *TAG = "OLED";

// Buffer del display (128x64 pixels / 8 = 1024 bytes)
static uint8_t oled_buffer[OLED_WIDTH * OLED_HEIGHT / 8];

// Font 5x8 simple para caracteres ASCII
static const uint8_t font5x8[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // ' ' (space)
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // '!'
    {0x00, 0x07, 0x00, 0x07, 0x00}, // '"'
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // '#'
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // '$'
    {0x23, 0x13, 0x08, 0x64, 0x62}, // '%'
    {0x36, 0x49, 0x55, 0x22, 0x50}, // '&'
    {0x00, 0x05, 0x03, 0x00, 0x00}, // '''
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // '('
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // ')'
    {0x08, 0x2A, 0x1C, 0x2A, 0x08}, // '*'
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // '+'
    {0x00, 0x50, 0x30, 0x00, 0x00}, // ','
    {0x08, 0x08, 0x08, 0x08, 0x08}, // '-'
    {0x00, 0x60, 0x60, 0x00, 0x00}, // '.'
    {0x20, 0x10, 0x08, 0x04, 0x02}, // '/'
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // '0'
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // '1'
    {0x42, 0x61, 0x51, 0x49, 0x46}, // '2'
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // '3'
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // '4'
    {0x27, 0x45, 0x45, 0x45, 0x39}, // '5'
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // '6'
    {0x01, 0x71, 0x09, 0x05, 0x03}, // '7'
    {0x36, 0x49, 0x49, 0x49, 0x36}, // '8'
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // '9'
    {0x00, 0x36, 0x36, 0x00, 0x00}, // ':'
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ';'
    {0x00, 0x08, 0x14, 0x22, 0x41}, // '<'
    {0x14, 0x14, 0x14, 0x14, 0x14}, // '='
    {0x41, 0x22, 0x14, 0x08, 0x00}, // '>'
    {0x02, 0x01, 0x51, 0x09, 0x06}, // '?'
    {0x32, 0x49, 0x79, 0x41, 0x3E}, // '@'
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // 'A'
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // 'B'
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // 'C'
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // 'D'
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // 'E'
    {0x7F, 0x09, 0x09, 0x01, 0x01}, // 'F'
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, // 'G'
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // 'H'
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // 'I'
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // 'J'
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // 'K'
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // 'L'
    {0x7F, 0x02, 0x04, 0x02, 0x7F}, // 'M'
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // 'N'
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // 'O'
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // 'P'
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // 'Q'
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // 'R'
    {0x46, 0x49, 0x49, 0x49, 0x31}, // 'S'
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // 'T'
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // 'U'
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // 'V'
    {0x7F, 0x20, 0x18, 0x20, 0x7F}, // 'W'
    {0x63, 0x14, 0x08, 0x14, 0x63}, // 'X'
    {0x03, 0x04, 0x78, 0x04, 0x03}, // 'Y'
    {0x61, 0x51, 0x49, 0x45, 0x43}, // 'Z'
};

// Función privada para enviar comando al OLED
static esp_err_t oled_write_command(uint8_t command) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x00, true); // Control byte: Co=0, D/C=0
    i2c_master_write_byte(cmd, command, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(OLED_I2C_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

// Función privada para enviar datos al OLED
static esp_err_t oled_write_data(uint8_t* data, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x40, true); // Control byte: Co=0, D/C=1
    i2c_master_write(cmd, data, len, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(OLED_I2C_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

// Inicializar I2C y el display OLED
esp_err_t oled_init(void) {
    esp_err_t ret = ESP_OK;
    
    ESP_LOGI(TAG, "Iniciando configuración I2C...");
    ESP_LOGI(TAG, "SDA: GPIO%d, SCL: GPIO%d", OLED_SDA_GPIO, OLED_SCL_GPIO);
    
    // Configurar I2C
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = OLED_SDA_GPIO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = OLED_SCL_GPIO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000, // 400kHz como en Arduino
        .clk_flags = 0,
    };
    
    ret = i2c_param_config(OLED_I2C_NUM, &conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error configurando I2C: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ret = i2c_driver_install(OLED_I2C_NUM, conf.mode, 0, 0, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error instalando driver I2C: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGI(TAG, "Driver I2C instalado correctamente");
    
    // Esperar un poco para que el display se inicialice
    vTaskDelay(pdMS_TO_TICKS(100));
    
    // Verificar si el dispositivo responde
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(OLED_I2C_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "No se pudo detectar el OLED en la dirección 0x%02X: %s", OLED_I2C_ADDRESS, esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGI(TAG, "OLED detectado en dirección 0x%02X", OLED_I2C_ADDRESS);
    
    // Enhanced SSD1306 initialization sequence
    ESP_LOGI(TAG, "Iniciando secuencia de inicialización del SSD1306...");
    
    ret = oled_write_command(SSD1306_DISPLAY_OFF);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error enviando comando DISPLAY_OFF: %s", esp_err_to_name(ret));
        return ret;
    }
    
    oled_write_command(SSD1306_SET_DISPLAY_CLOCK_DIV);
    oled_write_command(0x80);
    oled_write_command(SSD1306_SET_MULTIPLEX);
    oled_write_command(OLED_HEIGHT - 1);
    oled_write_command(SSD1306_SET_DISPLAY_OFFSET);
    oled_write_command(0x0);
    oled_write_command(SSD1306_SET_START_LINE | 0x0);
    oled_write_command(SSD1306_CHARGE_PUMP);
    oled_write_command(0x14);  // Enable charge pump
    oled_write_command(SSD1306_MEMORY_ADDR_MODE);
    oled_write_command(0x00);  // Horizontal addressing mode
    oled_write_command(SSD1306_SEG_REMAP | 0x1);
    oled_write_command(SSD1306_COM_SCAN_DEC);
    oled_write_command(SSD1306_SET_COM_PINS);
    oled_write_command(0x12);  // Para display 128x64
    oled_write_command(SSD1306_SET_CONTRAST);
    oled_write_command(0xFF);  // Contraste máximo como en Arduino (255)
    oled_write_command(SSD1306_SET_PRECHARGE);
    oled_write_command(0xF1);
    oled_write_command(SSD1306_SET_VCOM_DETECT);
    oled_write_command(0x40);
    oled_write_command(SSD1306_DISPLAY_ALL_ON_RESUME);
    oled_write_command(SSD1306_NORMAL_DISPLAY);
    
    vTaskDelay(pdMS_TO_TICKS(10)); // Esperar antes de encender
    
    ret = oled_write_command(SSD1306_DISPLAY_ON);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error enviando comando DISPLAY_ON: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGI(TAG, "Secuencia de inicialización completada");
    
    // Limpiar el display
    oled_clear();
    ret = oled_update_display();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error limpiando display: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGI(TAG, "OLED 0.42\" inicializado correctamente (72x40 pixels efectivos)");
    
    return ESP_OK;
}

// Limpiar el buffer del display
esp_err_t oled_clear(void) {
    memset(oled_buffer, 0, sizeof(oled_buffer));
    return ESP_OK;
}

// Función para dibujar un carácter en el buffer con offset
static void oled_draw_char(char c, int x, int y) {
    if (c < 32 || c > 90) c = 32; // Solo caracteres imprimibles
    
    // Aplicar offset para el display 0.42"
    int actual_x = x + OLED_OFFSET_X;
    int actual_y = y + OLED_OFFSET_Y;
    
    // Verificar límites de la resolución real
    if (x >= OLED_REAL_WIDTH || y >= OLED_REAL_HEIGHT) return;
    
    int char_index = c - 32;
    const uint8_t* char_data = font5x8[char_index];
    
    for (int i = 0; i < 5; i++) {
        if (actual_x + i >= OLED_WIDTH) break;
        
        uint8_t col_data = char_data[i];
        for (int j = 0; j < 8; j++) {
            if (actual_y + j >= OLED_HEIGHT) break;
            
            int buffer_pos = (actual_y + j) / 8 * OLED_WIDTH + (actual_x + i);
            int bit_pos = (actual_y + j) % 8;
            
            if (buffer_pos < sizeof(oled_buffer)) {
                if (col_data & (1 << j)) {
                    oled_buffer[buffer_pos] |= (1 << bit_pos);
                } else {
                    oled_buffer[buffer_pos] &= ~(1 << bit_pos);
                }
            }
        }
    }
}

// Función para dibujar un carácter escalado (30% más grande)
static void oled_draw_char_scaled(char c, int x, int y) {
    if (c < 32 || c > 90) c = 32; // Solo caracteres imprimibles
    
    // Aplicar offset para el display 0.42"
    int actual_x = x + OLED_OFFSET_X;
    int actual_y = y + OLED_OFFSET_Y;
    
    // Verificar límites de la resolución real (con escala)
    if (x >= OLED_REAL_WIDTH || y >= OLED_REAL_HEIGHT) return;
    
    int char_index = c - 32;
    const uint8_t* char_data = font5x8[char_index];
    
    // Escala aproximada del 30% más grande (1.3x)
    for (int i = 0; i < 5; i++) {
        uint8_t col_data = char_data[i];
        for (int j = 0; j < 8; j++) {
            if (col_data & (1 << j)) {
                // Dibujar píxel escalado (1x1 -> 1x2 aproximadamente)
                for (int sx = 0; sx <= 1; sx++) {
                    for (int sy = 0; sy <= 1; sy++) {
                        int scaled_x = actual_x + i + sx;
                        int scaled_y = actual_y + j + sy;
                        
                        if (scaled_x >= OLED_WIDTH || scaled_y >= OLED_HEIGHT) continue;
                        
                        int buffer_pos = (scaled_y / 8) * OLED_WIDTH + scaled_x;
                        int bit_pos = scaled_y % 8;
                        
                        if (buffer_pos < sizeof(oled_buffer)) {
                            oled_buffer[buffer_pos] |= (1 << bit_pos);
                        }
                    }
                }
            }
        }
    }
}

// Mostrar texto en el display
esp_err_t oled_display_text(const char* text, int x, int y) {
    int pos_x = x;
    
    for (int i = 0; text[i] != '\0'; i++) {
        if (pos_x >= OLED_REAL_WIDTH) break; // Usar resolución real
        
        oled_draw_char(text[i], pos_x, y);
        pos_x += 6; // 5 pixels + 1 espacio
    }
    
    return ESP_OK;
}

// Mostrar texto escalado en el display (30% más grande)
esp_err_t oled_display_text_scaled(const char* text, int x, int y) {
    int pos_x = x;
    
    for (int i = 0; text[i] != '\0'; i++) {
        if (pos_x >= OLED_REAL_WIDTH) break; // Usar resolución real
        
        oled_draw_char_scaled(text[i], pos_x, y);
        pos_x += 8; // 6 pixels escalados + 2 espacio
    }
    
    return ESP_OK;
}

// Función para dibujar un frame/borde en el área efectiva
static void oled_draw_frame(int x, int y, int width, int height) {
    // Aplicar offset
    int actual_x = x + OLED_OFFSET_X;
    int actual_y = y + OLED_OFFSET_Y;
    
    // Dibujar líneas horizontales (arriba y abajo)
    for (int i = 0; i < width; i++) {
        int top_pos = (actual_y / 8) * OLED_WIDTH + (actual_x + i);
        int bottom_pos = ((actual_y + height - 1) / 8) * OLED_WIDTH + (actual_x + i);
        
        if (top_pos < sizeof(oled_buffer)) {
            oled_buffer[top_pos] |= (1 << (actual_y % 8));
        }
        if (bottom_pos < sizeof(oled_buffer)) {
            oled_buffer[bottom_pos] |= (1 << ((actual_y + height - 1) % 8));
        }
    }
    
    // Dibujar líneas verticales (izquierda y derecha)
    for (int j = 0; j < height; j++) {
        int left_pos = ((actual_y + j) / 8) * OLED_WIDTH + actual_x;
        int right_pos = ((actual_y + j) / 8) * OLED_WIDTH + (actual_x + width - 1);
        
        if (left_pos < sizeof(oled_buffer)) {
            oled_buffer[left_pos] |= (1 << ((actual_y + j) % 8));
        }
        if (right_pos < sizeof(oled_buffer)) {
            oled_buffer[right_pos] |= (1 << ((actual_y + j) % 8));
        }
    }
}

// Función específica para mostrar "KAIXO"
esp_err_t oled_display_dataola(void) {
    ESP_LOGI(TAG, "Mostrando 'KAIXO' en el display 0.42\" (72x40)...");
    
    oled_clear();
    
    // Dibujar frame alrededor del área efectiva (como en Arduino)
    oled_draw_frame(0, 0, OLED_REAL_WIDTH, OLED_REAL_HEIGHT);
    
    // Mostrar "KAIXO" centrado en la resolución real (72x40) con texto escalado
    const char* text = "KAIXO";
    int text_width = strlen(text) * 8 - 2; // 8 pixels por carácter escalado menos 2
    int start_x = (OLED_REAL_WIDTH - text_width) / 2;  // Centrado en 72 pixels
    int start_y = (OLED_REAL_HEIGHT - 10) / 2;         // Centrado en 40 pixels (altura escalada ~10)
    
    ESP_LOGI(TAG, "Resolución real: %dx%d, Offset: (%d,%d)", 
             OLED_REAL_WIDTH, OLED_REAL_HEIGHT, OLED_OFFSET_X, OLED_OFFSET_Y);
    ESP_LOGI(TAG, "Posición del texto escalado: x=%d, y=%d (antes del offset)", start_x, start_y);
    
    oled_display_text_scaled(text, start_x, start_y);
    
    esp_err_t ret = oled_update_display();
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Texto 'KAIXO' con frame mostrado correctamente en display 0.42\"");
    } else {
        ESP_LOGE(TAG, "Error mostrando texto: %s", esp_err_to_name(ret));
    }
    
    return ret;
}

// Actualizar el display con el contenido del buffer
esp_err_t oled_update_display(void) {
    oled_write_command(SSD1306_SET_COLUMN_RANGE);
    oled_write_command(0);
    oled_write_command(OLED_WIDTH - 1);
    oled_write_command(SSD1306_SET_PAGE_RANGE);
    oled_write_command(0);
    oled_write_command((OLED_HEIGHT / 8) - 1);
    
    return oled_write_data(oled_buffer, sizeof(oled_buffer));
}

// Función para escanear dispositivos I2C
esp_err_t oled_scan_i2c_devices(void) {
    ESP_LOGI(TAG, "Escaneando dispositivos I2C...");
    
    int devices_found = 0;
    for (uint8_t addr = 1; addr < 127; addr++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);
        
        esp_err_t ret = i2c_master_cmd_begin(OLED_I2C_NUM, cmd, 100 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);
        
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "Dispositivo I2C encontrado en dirección: 0x%02X", addr);
            devices_found++;
        }
        
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    
    if (devices_found == 0) {
        ESP_LOGW(TAG, "No se encontraron dispositivos I2C");
    } else {
        ESP_LOGI(TAG, "Total de dispositivos I2C encontrados: %d", devices_found);
    }
    
    return ESP_OK;
}

// Función de prueba simple - similar al patrón de Arduino
esp_err_t oled_test_pattern(void) {
    ESP_LOGI(TAG, "Mostrando patrón de prueba...");
    
    oled_clear();
    
    // Llenar toda el área efectiva con un patrón
    for (int x = 0; x < OLED_REAL_WIDTH; x++) {
        for (int y = 0; y < OLED_REAL_HEIGHT; y++) {
            int actual_x = x + OLED_OFFSET_X;
            int actual_y = y + OLED_OFFSET_Y;
            
            if (actual_x < OLED_WIDTH && actual_y < OLED_HEIGHT) {
                int buffer_pos = (actual_y / 8) * OLED_WIDTH + actual_x;
                int bit_pos = actual_y % 8;
                
                if (buffer_pos < sizeof(oled_buffer)) {
                    // Patrón de tablero de ajedrez
                    if ((x + y) % 4 == 0) {
                        oled_buffer[buffer_pos] |= (1 << bit_pos);
                    }
                }
            }
        }
    }
    
    esp_err_t ret = oled_update_display();
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Patrón de prueba mostrado correctamente");
    } else {
        ESP_LOGE(TAG, "Error mostrando patrón: %s", esp_err_to_name(ret));
    }
    
    return ret;
}