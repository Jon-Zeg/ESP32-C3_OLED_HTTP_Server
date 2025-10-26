idf_component_register(SRCS "oled_display.c" "softap_example_main.c"
                    PRIV_REQUIRES esp_wifi nvs_flash esp_http_server driver
                    INCLUDE_DIRS ".")
