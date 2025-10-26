# ESP32-C3 OLED Display Project

Este proyecto implementa un sistema completo con ESP32-C3 que incluye display OLED, servidor HTTP y control de LED.

## Características

- **Display OLED SSD1306 0.42"**: Muestra el texto "KAIXO" centrado con letras escaladas
- **WiFi Access Point**: Crea un punto de acceso WiFi llamado "ESP32-SoftAP"  
- **Servidor HTTP**: Servidor web en la IP 192.168.4.1 con página personalizada
- **LED de estado**: LED en GPIO8 que parpadea cada 500ms
- **Programación USB**: Soporte nativo para programación via USB Serial/JTAG

## Hardware utilizado

- **ESP32-C3**: Microcontrolador principal (QFN32, 4MB flash)
- **Display OLED**: SSD1306 0.42" (72x40 pixels efectivos)
  - SDA: GPIO5
  - SCL: GPIO6  
  - Dirección I2C: 0x3C
- **LED**: Conectado a GPIO8

## Configuración

### Conexiones I2C (Display OLED)
```
ESP32-C3    →    OLED SSD1306
GPIO5       →    SDA
GPIO6       →    SCL
3.3V        →    VCC  
GND         →    GND
```

### LED de estado
```
ESP32-C3    →    LED
GPIO8       →    Ánodo (+)
GND         →    Cátodo (-) con resistencia
```

## Funcionalidades

### 1. Display OLED
- Inicialización automática del SSD1306
- Texto "KAIXO" centrado con fuente escalada (30% más grande)
- Frame decorativo alrededor del área de visualización
- Resolución efectiva: 72x40 pixels con offset (28,24)

### 2. WiFi Access Point
- **SSID**: ESP32-SoftAP
- **Password**: 123456789
- **IP**: 192.168.4.1
- **Rango DHCP**: 192.168.4.2 - 192.168.4.10

### 3. Servidor HTTP
- Página principal en `http://192.168.4.1/`
- Mensaje personalizado: "KAIXO test http server"
- Respuesta HTTP básica

### 4. LED indicador
- Parpadeo cada 500ms (500ms ON, 500ms OFF)
- Indica que el sistema está funcionando correctamente

## Compilación y programación

### Requisitos
- ESP-IDF v5.5.1
- Puerto USB para programación (normalmente `/dev/ttyACM0`)

### Comandos
```bash
# Compilar
idf.py build

# Flashear
idf.py -p /dev/ttyACM0 flash

# Monitor serie
idf.py -p /dev/ttyACM0 monitor
```

## Estructura del proyecto

```
├── main/
│   ├── CMakeLists.txt
│   ├── softap_example_main.c    # Archivo principal
│   ├── oled_display.c           # Driver del display OLED
│   ├── oled_display.h           # Definiciones del display
│   └── Kconfig.projbuild
├── CMakeLists.txt
├── sdkconfig
└── README.md
```

## Logs del sistema

El sistema proporciona información detallada por el puerto serie:
- Estado de inicialización del WiFi AP
- Confirmación de inicialización del OLED
- Estado del servidor HTTP
- Información de conexiones de clientes

## Configuración avanzada

### Modificar el texto del display
Editar en `oled_display.c`, función `oled_display_dataola()`:
```c
const char* text = "KAIXO";  // Cambiar por el texto deseado
```

### Ajustar offset del display
Modificar en `oled_display.h`:
```c
#define OLED_OFFSET_X       28   // Offset horizontal
#define OLED_OFFSET_Y       24   // Offset vertical
```

### Cambiar frecuencia del LED
Modificar en `softap_example_main.c`, función `led_blink_task()`:
```c
vTaskDelay(500 / portTICK_PERIOD_MS);  // Cambiar 500 por el valor deseado
```

## Licencia

Este proyecto es de código abierto y se puede usar libremente para propósitos educativos y comerciales.

## Autor

Desarrollado para ESP32-C3 con ESP-IDF 5.5.1

Basado en la publicación del log de  Kevin : https://emalliab.wordpress.com/2025/02/12/esp32-c3-0-42-oled/. Muchas gracias
