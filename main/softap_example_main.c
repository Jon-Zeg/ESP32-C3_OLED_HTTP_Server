/*  WiFi softAP Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_server.h"
#include "oled_display.h"
#include "driver/i2c.h"
#include "driver/gpio.h"

#include "lwip/err.h"
#include "lwip/sys.h"

/* The examples use WiFi configuration that you can set via project configuration menu.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/

#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_WIFI_CHANNEL   CONFIG_ESP_WIFI_CHANNEL
#define EXAMPLE_MAX_STA_CONN       CONFIG_ESP_MAX_STA_CONN

#if CONFIG_ESP_GTK_REKEYING_ENABLE
#define EXAMPLE_GTK_REKEY_INTERVAL CONFIG_ESP_GTK_REKEY_INTERVAL
#else
#define EXAMPLE_GTK_REKEY_INTERVAL 0
#endif

// LED configuration
#define LED_GPIO    8
#define LED_ON_TIME_MS  500
#define LED_OFF_TIME_MS 500

static const char *TAG = "wifi softAP";

/* Function to handle the main HTTP server page */
static esp_err_t hello_get_handler(httpd_req_t *req)
{
    const char* resp_str = "<!DOCTYPE html>"
                          "<html>"
                          "<head><title>DataOla ESP32-C3</title></head>"
                          "<body style='font-family: Arial, sans-serif; margin: 40px;'>"
                          "<h1 style='color: #2E86AB;'>DATAOLA Test HTTP Server</h1>"
                          "<p>🚀 ¡Servidor HTTP funcionando en ESP32-C3!</p>"
                          "<p>🔌 Conectado vía USB Serial/JTAG</p>"
                          "<p>📡 Access Point configurado correctamente</p>"
                          "<p>📺 Display OLED mostrando 'DATAOLA'</p>"
                          "<p>💡 LED parpadeando en GPIO8 (500ms on/off)</p>"
                          "<hr>"
                          "<h2>Información del sistema:</h2>"
                          "<ul>"
                          "<li>Chip: ESP32-C3</li>"
                          "<li>Display: OLED 0.42\" SSD1306 (72x40 efectivo)</li>"
                          "<li>I2C: SDA=GPIO5, SCL=GPIO6</li>"
                          "<li>LED: GPIO8 (parpadeo 500ms)</li>"
                          "</ul>"
                          "</body>"
                          "</html>";
    
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* URI handler for the main page */
static const httpd_uri_t hello = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = hello_get_handler,
    .user_ctx  = NULL
};

/* Function to initialize the HTTP server */
static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the HTTP server
    ESP_LOGI(TAG, "Starting HTTP server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Register URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &hello);
        return server;
    }

    ESP_LOGI(TAG, "Error starting HTTP server!");
    return NULL;
}

/* Task to blink the LED */
static void led_blink_task(void *pvParameters)
{
    // Configure LED GPIO as output
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    
    ESP_LOGI(TAG, "Starting LED blink on GPIO%d", LED_GPIO);
    
    bool led_state = false;
    
    while (1) {
        // Cambiar estado del LED
        led_state = !led_state;
        gpio_set_level(LED_GPIO, led_state ? 1 : 0);
        
        ESP_LOGI(TAG, "LED %s", led_state ? "ON" : "OFF");
        
        // Wait according to state
        if (led_state) {
            vTaskDelay(pdMS_TO_TICKS(LED_ON_TIME_MS));
        } else {
            vTaskDelay(pdMS_TO_TICKS(LED_OFF_TIME_MS));
        }
    }
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d, reason=%d",
                 MAC2STR(event->mac), event->aid, event->reason);
    }
}

void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .channel = EXAMPLE_ESP_WIFI_CHANNEL,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
#ifdef CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT
            .authmode = WIFI_AUTH_WPA3_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
#else /* CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT */
            .authmode = WIFI_AUTH_WPA2_PSK,
#endif
            .pmf_cfg = {
                    .required = true,
            },
#ifdef CONFIG_ESP_WIFI_BSS_MAX_IDLE_SUPPORT
            .bss_max_idle_cfg = {
                .period = WIFI_AP_DEFAULT_MAX_IDLE_PERIOD,
                .protected_keep_alive = 1,
            },
#endif
            .gtk_rekey_interval = EXAMPLE_GTK_REKEY_INTERVAL,
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS, EXAMPLE_ESP_WIFI_CHANNEL);
}

void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize OLED display and show "KAIXO"
    ESP_LOGI(TAG, "Initializing OLED display...");
    
    // Call oled_init directly which handles all I2C configuration
    if (oled_init() == ESP_OK) {
        ESP_LOGI(TAG, "OLED display initialized correctly");
        
        // Scan I2C devices for debug
        oled_scan_i2c_devices();
        
        // First show test pattern
        ESP_LOGI(TAG, "Showing test pattern for 3 seconds...");
        oled_test_pattern();
        vTaskDelay(pdMS_TO_TICKS(3000));
        
        // Then show KAIXO
        oled_display_dataola();
        ESP_LOGI(TAG, "Text 'KAIXO' displayed on screen");
    } else {
        ESP_LOGE(TAG, "Error initializing OLED display");
    }

    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
    wifi_init_softap();
    
    // Initialize HTTP server
    httpd_handle_t server = start_webserver();
    if (server) {
        ESP_LOGI(TAG, "HTTP server started successfully");
        ESP_LOGI(TAG, "Connect to WiFi network '%s' and visit http://192.168.4.1", EXAMPLE_ESP_WIFI_SSID);
    } else {
        ESP_LOGE(TAG, "Error starting HTTP server");
    }
    
    // Create task for LED blinking
    ESP_LOGI(TAG, "Creating LED blink task...");
    xTaskCreate(led_blink_task, "led_blink", 2048, NULL, 5, NULL);
    ESP_LOGI(TAG, "LED task created - GPIO%d blinking every %dms", LED_GPIO, LED_ON_TIME_MS);
}
