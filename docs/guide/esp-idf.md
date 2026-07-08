# Guía de ESP-IDF

ESP-IDF (Espressif IoT Development Framework) es el framework oficial de desarrollo para ESP32-C6.

## Comandos básicos

### Configurar target

```bash
idf.py set-target esp32c6
```

### Compilar proyecto

```bash
idf.py build
```

### Flashear a la placa

```bash
idf.py -p /dev/ttyUSB0 flash
```

### Monitorear salida serial

```bash
idf.py -p /dev/ttyUSB0 monitor
```

### Todo en uno (compilar, flashear y monitorear)

```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

### Limpiar compilación

```bash
idf.py fullclean
```

### Configurar proyecto

```bash
idf.py menuconfig
```

## Estructura de un componente

Los componentes son módulos reutilizables en ESP-IDF:

```
components/
└── mi_sensor/
    ├── CMakeLists.txt
    ├── include/
    │   └── mi_sensor.h
    └── mi_sensor.c
```

**CMakeLists.txt:**
```cmake
idf_component_register(
    SRCS "mi_sensor.c"
    INCLUDE_DIRS "include"
    REQUIRES "driver"
)
```

## Sistema de logging

ESP-IDF incluye un sistema de logging por niveles:

```c
#include "esp_log.h"

static const char *TAG = "MI_APP";

void app_main(void)
{
    ESP_LOGE(TAG, "Error crítico");    // Rojo
    ESP_LOGW(TAG, "Advertencia");      // Amarillo
    ESP_LOGI(TAG, "Información");      // Verde
    ESP_LOGD(TAG, "Debug");            // 
    ESP_LOGV(TAG, "Verbose");          // Gris
}
```

Configurar nivel de log:
```bash
idf.py menuconfig
# Component config → Log output → Default log verbosity
```

## FreeRTOS Tasks

ESP-IDF usa FreeRTOS para multitarea:

```c
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void task_function(void *pvParameters)
{
    while (1) {
        // Hacer algo
        vTaskDelay(pdMS_TO_TICKS(1000)); // Esperar 1 segundo
    }
}

void app_main(void)
{
    xTaskCreate(
        task_function,      // Función de la tarea
        "MiTarea",         // Nombre
        2048,              // Stack size (bytes)
        NULL,              // Parámetros
        5,                 // Prioridad
        NULL               // Handle
    );
}
```

## NVS (Non-Volatile Storage)

Guardar datos persistentes en flash:

```c
#include "nvs_flash.h"
#include "nvs.h"

void app_main(void)
{
    // Inicializar NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || 
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Abrir namespace
    nvs_handle_t my_handle;
    ret = nvs_open("storage", NVS_READWRITE, &my_handle);
    
    // Escribir
    int32_t value = 42;
    nvs_set_i32(my_handle, "mi_valor", value);
    nvs_commit(my_handle);
    
    // Leer
    int32_t read_value;
    nvs_get_i32(my_handle, "mi_valor", &read_value);
    
    nvs_close(my_handle);
}
```

## Manejo de GPIOs

```c
#include "driver/gpio.h"

#define LED_PIN GPIO_NUM_8

void app_main(void)
{
    // Configurar como salida
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    
    while (1) {
        gpio_set_level(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_set_level(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
```

## Event Loop

Sistema de eventos para comunicación entre componentes:

```c
#include "esp_event.h"

ESP_EVENT_DEFINE_BASE(MY_EVENT_BASE);

enum {
    MY_EVENT_ID_1,
    MY_EVENT_ID_2
};

static void event_handler(void *arg, esp_event_base_t event_base,
                         int32_t event_id, void *event_data)
{
    if (event_base == MY_EVENT_BASE) {
        switch (event_id) {
            case MY_EVENT_ID_1:
                ESP_LOGI("EVENT", "Evento 1 recibido");
                break;
        }
    }
}

void app_main(void)
{
    // Crear event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    // Registrar handler
    ESP_ERROR_CHECK(esp_event_handler_register(
        MY_EVENT_BASE, MY_EVENT_ID_1, event_handler, NULL));
    
    // Publicar evento
    ESP_ERROR_CHECK(esp_event_post(
        MY_EVENT_BASE, MY_EVENT_ID_1, NULL, 0, portMAX_DELAY));
}
```

## Buenas prácticas

### 1. Verificar errores

```c
esp_err_t ret = gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Error configurando GPIO: %s", esp_err_to_name(ret));
    return;
}

// O usar macro para abortar en error
ESP_ERROR_CHECK(gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT));
```

### 2. Gestión de memoria

```c
// Usar heap_caps para memoria específica
char *buffer = heap_caps_malloc(1024, MALLOC_CAP_DMA);
if (buffer == NULL) {
    ESP_LOGE(TAG, "No hay memoria disponible");
    return;
}

// Siempre liberar
free(buffer);
```

### 3. Watchdog timer

```c
#include "esp_task_wdt.h"

void long_task(void *pvParameters)
{
    // Agregar tarea al watchdog
    esp_task_wdt_add(NULL);
    
    while (1) {
        // Trabajo pesado aquí
        
        // Resetear watchdog
        esp_task_wdt_reset();
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
```

### 4. Particiones de flash

Ver particiones disponibles:
```bash
idf.py partition-table
```

Archivo `partitions.csv` personalizado:
```csv
# Name,   Type, SubType, Offset,  Size, Flags
nvs,      data, nvs,     0x9000,  0x6000,
phy_init, data, phy,     0xf000,  0x1000,
factory,  app,  factory, 0x10000, 1M,
storage,  data, spiffs,  0x110000, 1M,
```

## Debugging

### Monitor serial avanzado

```bash
# Decodificar stack traces
idf.py -p /dev/ttyUSB0 monitor

# Filtrar logs
idf.py -p /dev/ttyUSB0 monitor --print_filter="MI_TAG:I *:E"
```

### GDB debugging

```bash
# Terminal 1: OpenOCD
openocd -f board/esp32c6-builtin.cfg

# Terminal 2: GDB
idf.py gdb
```

### Core dump

Configurar en menuconfig:
```
Component config → Core dump → Data destination → Flash
```

Leer dump después de crash:
```bash
idf.py coredump-info
```

## Referencias

- [API Reference](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/index.html)
- [FreeRTOS Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-guides/freertos-smp.html)
- [Build System](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-guides/build-system.html)
