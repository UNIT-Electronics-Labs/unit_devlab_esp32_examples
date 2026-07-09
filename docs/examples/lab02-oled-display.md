# Lab 2: Visualización en Pantalla OLED

En esta práctica aprenderás a mostrar datos de sensores en una pantalla OLED mediante I²C.

## Objetivos

- Configurar pantalla OLED SSD1306
- Mostrar texto y gráficos
- Visualizar datos de sensores en tiempo real
- Crear interfaz de usuario simple

## Materiales

- Placa Pulsar C6 (ESP32-C6)
- Pantalla OLED 128x64 SSD1306 (I²C)
- Sensor BME280 (opcional, del Lab 1)
- Cables jumper
- Protoboard

## Conexiones

```
ESP32-C6 Pulsar C6    OLED SSD1306    BME280 (opcional)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
3.3V                  VCC             VCC
GND                   GND             GND
GPIO6 (SDA)           SDA             SDA
GPIO7 (SCL)           SCL             SCL
```

::: tip Conexión en paralelo
Como I²C es un bus, puedes conectar múltiples dispositivos en paralelo (SDA con SDA, SCL con SCL).
:::

## Instalación de bibliotecas Arduino

Instala estas librerías desde **Tools > Manage Libraries**:

- `Adafruit SSD1306`
- `Adafruit GFX Library`
- `Adafruit BME280 Library` si quieres integrar el sensor del Lab 1

## Código base Arduino

```cpp
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int SDA_PIN = 6;
const int SCL_PIN = 7;

Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("No se encontro OLED SSD1306 en 0x3C");
    while (true) delay(10);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Pulsar C6");
  display.println("OLED lista");
  display.display();
}

void loop() {
  static int counter = 0;

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Lab 2 OLED");
  display.print("Contador: ");
  display.println(counter++);
  display.display();

  delay(1000);
}
```

## Referencia avanzada ESP-IDF

### CMakeLists.txt del proyecto

```cmake
idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES driver esp_lcd
)
```

### Parte 1: Inicialización y texto básico

```c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_log.h"

#define I2C_MASTER_SCL_IO    7
#define I2C_MASTER_SDA_IO    6
#define I2C_MASTER_NUM       I2C_NUM_0
#define I2C_MASTER_FREQ_HZ   400000

#define LCD_PIXEL_CLOCK_HZ   (400 * 1000)
#define LCD_H_RES            128
#define LCD_V_RES            64
#define LCD_CMD_BITS         8
#define LCD_PARAM_BITS       8

static const char *TAG = "OLED";

esp_lcd_panel_handle_t panel_handle = NULL;

void i2c_master_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));
}

void oled_init(void)
{
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = 0x3C,  // Dirección I2C típica (0x3C o 0x3D)
        .control_phase_bytes = 1,
        .lcd_cmd_bits = LCD_CMD_BITS,
        .lcd_param_bits = LCD_PARAM_BITS,
        .dc_bit_offset = 6,
    };
    
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)I2C_MASTER_NUM, 
                                             &io_config, &io_handle));
    
    esp_lcd_panel_dev_config_t panel_config = {
        .bits_per_pixel = 1,
        .reset_gpio_num = -1,
    };
    
    ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    
    ESP_LOGI(TAG, "OLED inicializado");
}

// Función simple para dibujar píxel
void draw_pixel(int x, int y, bool color)
{
    if (x >= 0 && x < LCD_H_RES && y >= 0 && y < LCD_V_RES) {
        uint8_t buffer[LCD_H_RES * LCD_V_RES / 8] = {0};
        int byte_index = x + (y / 8) * LCD_H_RES;
        int bit_index = y % 8;
        
        if (color) {
            buffer[byte_index] |= (1 << bit_index);
        }
        
        esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, LCD_H_RES, LCD_V_RES, buffer);
    }
}

// Limpiar pantalla
void clear_screen(void)
{
    uint8_t buffer[LCD_H_RES * LCD_V_RES / 8] = {0};
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, LCD_H_RES, LCD_V_RES, buffer);
}

void app_main(void)
{
    i2c_master_init();
    oled_init();
    
    // Limpiar pantalla
    clear_screen();
    
    ESP_LOGI(TAG, "Pantalla lista");
    
    // Patrón de prueba: líneas horizontales
    uint8_t buffer[LCD_H_RES * LCD_V_RES / 8] = {0};
    
    for (int y = 0; y < LCD_V_RES; y += 8) {
        for (int x = 0; x < LCD_H_RES; x++) {
            int byte_index = x + (y / 8) * LCD_H_RES;
            buffer[byte_index] = 0xFF;  // Línea completa
        }
    }
    
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, LCD_H_RES, LCD_V_RES, buffer);
}
```

### Parte 2: Mostrar texto (usando biblioteca lvgl)

Para mostrar texto fácilmente, es mejor usar LVGL:

```bash
idf.py add-dependency "lvgl/lvgl^8.3.0"
```

```c
#include "lvgl.h"

#define LCD_H_RES 128
#define LCD_V_RES 64

static lv_disp_draw_buf_t disp_buf;
static lv_disp_drv_t disp_drv;
static lv_color_t buf[LCD_H_RES * 10];

void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    int w = (area->x2 - area->x1 + 1);
    int h = (area->y2 - area->y1 + 1);
    
    // Convertir buffer LVGL a formato SSD1306
    uint8_t *buffer = malloc(LCD_H_RES * LCD_V_RES / 8);
    memset(buffer, 0, LCD_H_RES * LCD_V_RES / 8);
    
    for (int y = area->y1; y <= area->y2; y++) {
        for (int x = area->x1; x <= area->x2; x++) {
            lv_color_t color = color_map[(y - area->y1) * w + (x - area->x1)];
            if (lv_color_brightness(color) > 128) {
                int byte_index = x + (y / 8) * LCD_H_RES;
                int bit_index = y % 8;
                buffer[byte_index] |= (1 << bit_index);
            }
        }
    }
    
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, LCD_H_RES, LCD_V_RES, buffer);
    free(buffer);
    
    lv_disp_flush_ready(drv);
}

void lvgl_init(void)
{
    lv_init();
    
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, LCD_H_RES * 10);
    
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LCD_H_RES;
    disp_drv.ver_res = LCD_V_RES;
    disp_drv.flush_cb = lvgl_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    
    lv_disp_drv_register(&disp_drv);
}

void display_sensor_data(float temp, float hum, float pres)
{
    lv_obj_t *label = lv_label_create(lv_scr_act());
    
    char text[128];
    snprintf(text, sizeof(text), 
            "Temp: %.1f C\nHum: %.1f %%\nPres: %.0f hPa", 
            temp, hum, pres);
    
    lv_label_set_text(label, text);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

void app_main(void)
{
    i2c_master_init();
    oled_init();
    lvgl_init();
    
    // Mostrar datos de ejemplo
    display_sensor_data(25.5, 65.2, 1013.0);
    
    // LVGL task loop
    while (1) {
        lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
```

### Parte 3: Integración con sensor BME280

```c
#include "bme280.h"  // Del Lab 1

void sensor_display_task(void *pvParameters)
{
    lv_obj_t *temp_label = lv_label_create(lv_scr_act());
    lv_obj_set_pos(temp_label, 5, 5);
    
    lv_obj_t *hum_label = lv_label_create(lv_scr_act());
    lv_obj_set_pos(hum_label, 5, 25);
    
    lv_obj_t *pres_label = lv_label_create(lv_scr_act());
    lv_obj_set_pos(pres_label, 5, 45);
    
    while (1) {
        float temperature, pressure, humidity;
        bme280_read_data(&temperature, &pressure, &humidity);
        
        char buffer[32];
        
        snprintf(buffer, sizeof(buffer), "Temp: %.1f C", temperature);
        lv_label_set_text(temp_label, buffer);
        
        snprintf(buffer, sizeof(buffer), "Hum: %.1f %%", humidity);
        lv_label_set_text(hum_label, buffer);
        
        snprintf(buffer, sizeof(buffer), "Pres: %.0f hPa", pressure);
        lv_label_set_text(pres_label, buffer);
        
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main(void)
{
    i2c_master_init();
    oled_init();
    lvgl_init();
    bme280_init();  // Del Lab 1
    
    xTaskCreate(sensor_display_task, "display", 4096, NULL, 5, NULL);
    
    while (1) {
        lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
```

## Ejercicios

### Ejercicio 1: Gráfico de barras
Crea un gráfico de barras que muestre la temperatura en los últimos 10 segundos.

### Ejercicio 2: Menú de navegación
Implementa un menú con botones para cambiar entre diferentes vistas (temperatura, humedad, presión).

### Ejercicio 3: Iconos
Muestra iconos según las condiciones (sol si T>25°C, nube si humedad >70%).

## Alternativas sin LVGL

Si prefieres no usar LVGL, puedes usar bibliotecas más simples como u8g2:

```bash
git clone https://github.com/olikraus/u8g2.git components/u8g2
```

```c
#include "u8g2.h"

u8g2_t u8g2;

void u8g2_init_display(void)
{
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, 
                                           u8x8_byte_esp32_i2c, 
                                           u8x8_gpio_and_delay_esp32);
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
}

void display_text(const char *text)
{
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_ncenB14_tr);
    u8g2_DrawStr(&u8g2, 0, 15, text);
    u8g2_SendBuffer(&u8g2);
}
```

## Solución de problemas

| Problema | Solución |
|----------|----------|
| Pantalla no enciende | Verificar dirección (0x3C vs 0x3D), conexiones |
| Texto cortado | Ajustar tamaño de fuente, verificar coordenadas |
| Actualización lenta | Optimizar refresh rate, usar double buffering |
| Pantalla invertida | Cambiar orientación en configuración |

## Próximos pasos

- [Lab 3: ADC analógico](./lab03-adc.md)
- [Guía I²C](../guide/i2c.md)
- [LVGL Documentation](https://docs.lvgl.io/)

## Referencias

- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
- [SSD1306 Datasheet](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)
- [u8g2 Library](https://github.com/olikraus/u8g2)
- [LVGL for ESP32](https://docs.lvgl.io/latest/en/html/get-started/espressif.html) - referencia avanzada
