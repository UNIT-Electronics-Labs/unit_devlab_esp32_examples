# Lab 3: Conversión Analógica-Digital (ADC)

En esta práctica aprenderás a leer señales analógicas usando el ADC del ESP32-C6.

## Objetivos

- Configurar y usar el ADC del ESP32-C6
- Leer sensores analógicos
- Calibrar el ADC para mayor precisión
- Implementar filtrado de ruido
- Convertir valores ADC a unidades físicas

## Materiales

- Placa Pulsar C6 (ESP32-C6)
- Potenciómetro 10kΩ
- Sensor LM35 (temperatura analógico)
- Fotoresistencia (LDR) + resistencia 10kΩ
- Cables jumper
- Protoboard

## Teoría: ADC en ESP32-C6

El ADC del ESP32-C6 tiene las siguientes características:
- **Resolución**: 12 bits (4096 niveles: 0-4095)
- **Canales**: 7 (GPIO0-GPIO6)
- **Rango**: 0-2.5V (con atenuación 11dB)

## Práctica 1: Lectura de potenciómetro

### Conexiones

```
Potenciómetro 10kΩ
━━━━━━━━━━━━━━━━━━
Terminal 1 ────── 3.3V
Terminal 2 (cursor) ── GPIO0 (ADC1_CHANNEL_0)
Terminal 3 ────── GND
```

### Código

```c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"

#define ADC_CHANNEL     ADC_CHANNEL_0  // GPIO0
#define ADC_ATTEN       ADC_ATTEN_DB_11
#define ADC_WIDTH       ADC_BITWIDTH_12

static const char *TAG = "ADC_EXAMPLE";

void app_main(void)
{
    // 1. Configurar ADC
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc1_handle));
    
    // 2. Configurar canal
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_WIDTH,
        .atten = ADC_ATTEN,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL, &config));
    
    // 3. Configurar calibración
    adc_cali_handle_t adc_cali_handle = NULL;
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN,
        .bitwidth = ADC_WIDTH,
    };
    
    esp_err_t ret = adc_cali_create_scheme_line_fitting(&cali_config, &adc_cali_handle);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Calibración habilitada");
    }
    
    // 4. Loop de lectura
    while (1) {
        int adc_raw;
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL, &adc_raw));
        
        // Convertir a voltaje
        int voltage_mv;
        if (adc_cali_handle) {
            ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc_cali_handle, adc_raw, &voltage_mv));
            ESP_LOGI(TAG, "ADC: %d, Voltaje: %d mV, Porcentaje: %.1f%%", 
                    adc_raw, voltage_mv, (voltage_mv / 2500.0) * 100);
        } else {
            float voltage = (adc_raw / 4095.0) * 2.5;
            ESP_LOGI(TAG, "ADC: %d, Voltaje: %.3f V", adc_raw, voltage);
        }
        
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
```

## Práctica 2: Sensor de temperatura LM35

El LM35 produce 10mV/°C.

### Conexiones

```
LM35
━━━━━━━━━━━━━━━━
VCC (izq) ── 3.3V
Vout (centro) ── GPIO1 (ADC1_CHANNEL_1)
GND (der) ── GND
```

::: warning Rango del LM35
El LM35 puede producir hasta 1.5V (150°C). Como el ESP32-C6 ADC con atenuación 11dB mide hasta 2.5V, esto está dentro del rango seguro.
:::

### Código

```c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"

#define LM35_CHANNEL    ADC_CHANNEL_1  // GPIO1
#define ADC_ATTEN       ADC_ATTEN_DB_11
#define ADC_WIDTH       ADC_BITWIDTH_12

static const char *TAG = "LM35";

float read_temperature_lm35(adc_oneshot_unit_handle_t adc_handle, 
                            adc_cali_handle_t cali_handle)
{
    int adc_raw;
    adc_oneshot_read(adc_handle, LM35_CHANNEL, &adc_raw);
    
    int voltage_mv;
    adc_cali_raw_to_voltage(cali_handle, adc_raw, &voltage_mv);
    
    // LM35: 10mV/°C
    float temperature = voltage_mv / 10.0;
    
    return temperature;
}

// Promedio de múltiples muestras para reducir ruido
float read_temperature_averaged(adc_oneshot_unit_handle_t adc_handle, 
                                adc_cali_handle_t cali_handle, int samples)
{
    float sum = 0;
    
    for (int i = 0; i < samples; i++) {
        sum += read_temperature_lm35(adc_handle, cali_handle);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    
    return sum / samples;
}

void app_main(void)
{
    // Inicializar ADC (igual que antes)
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config = {.unit_id = ADC_UNIT_1};
    adc_oneshot_new_unit(&init_config, &adc1_handle);
    
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_WIDTH,
        .atten = ADC_ATTEN,
    };
    adc_oneshot_config_channel(adc1_handle, LM35_CHANNEL, &config);
    
    // Calibración
    adc_cali_handle_t adc_cali_handle;
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN,
        .bitwidth = ADC_WIDTH,
    };
    adc_cali_create_scheme_line_fitting(&cali_config, &adc_cali_handle);
    
    ESP_LOGI(TAG, "LM35 inicializado");
    
    while (1) {
        float temperature = read_temperature_averaged(adc1_handle, adc_cali_handle, 10);
        ESP_LOGI(TAG, "Temperatura: %.2f °C", temperature);
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

## Práctica 3: Fotoresistencia (LDR)

### Conexiones (divisor de voltaje)

```
3.3V ──┬── LDR (fotoresistencia)
       │
       ├── GPIO2 (ADC1_CHANNEL_2)
       │
       └── 10kΩ
       │
      GND
```

### Código

```c
#include <stdio.h>
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_log.h"

#define LDR_CHANNEL     ADC_CHANNEL_2  // GPIO2

static const char *TAG = "LDR";

int read_light_level(adc_oneshot_unit_handle_t adc_handle, 
                     adc_cali_handle_t cali_handle)
{
    int adc_raw;
    adc_oneshot_read(adc_handle, LDR_CHANNEL, &adc_raw);
    
    int voltage_mv;
    adc_cali_raw_to_voltage(cali_handle, adc_raw, &voltage_mv);
    
    // Convertir a porcentaje (0-100%)
    int light_percent = (voltage_mv * 100) / 2500;
    
    return light_percent;
}

const char* get_light_description(int percent)
{
    if (percent < 20) return "Muy oscuro";
    if (percent < 40) return "Oscuro";
    if (percent < 60) return "Normal";
    if (percent < 80) return "Brillante";
    return "Muy brillante";
}

void app_main(void)
{
    // Inicializar ADC (código similar a ejemplos anteriores)
    adc_oneshot_unit_handle_t adc1_handle;
    adc_cali_handle_t adc_cali_handle;
    
    // ... código de inicialización ...
    
    while (1) {
        int light = read_light_level(adc1_handle, adc_cali_handle);
        ESP_LOGI(TAG, "Luz: %d%% - %s", light, get_light_description(light));
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

## Práctica 4: Filtrado avanzado

### Filtro de mediana (elimina picos)

```c
int compare_ints(const void *a, const void *b)
{
    return (*(int*)a - *(int*)b);
}

int read_adc_median_filter(adc_oneshot_unit_handle_t adc_handle, 
                           adc_channel_t channel, int samples)
{
    int readings[samples];
    
    for (int i = 0; i < samples; i++) {
        adc_oneshot_read(adc_handle, channel, &readings[i]);
    }
    
    // Ordenar
    qsort(readings, samples, sizeof(int), compare_ints);
    
    // Retornar mediana
    return readings[samples / 2];
}
```

### Filtro paso bajo (suaviza señal)

```c
float lowpass_filter(float new_value, float filtered_value, float alpha)
{
    return alpha * new_value + (1.0 - alpha) * filtered_value;
}

void app_main(void)
{
    // ... inicialización ...
    
    float filtered_value = 0;
    float alpha = 0.1;  // Factor de suavizado (0-1)
    
    while (1) {
        int adc_raw;
        adc_oneshot_read(adc1_handle, ADC_CHANNEL, &adc_raw);
        
        filtered_value = lowpass_filter(adc_raw, filtered_value, alpha);
        
        ESP_LOGI(TAG, "Crudo: %d, Filtrado: %.0f", adc_raw, filtered_value);
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
```

## Ejercicios

### Ejercicio 1: Batería
Mide el voltaje de una batería usando un divisor de voltaje. Calcula el porcentaje de carga.

### Ejercicio 2: Multi-canal
Lee 3 sensores analógicos simultáneamente (potenciómetro, LM35, LDR) y muestra en pantalla OLED.

### Ejercicio 3: Data logger
Guarda lecturas del ADC en NVS cada minuto y muestra historial.

### Ejercicio 4: Alertas
Implementa un sistema de alertas que encienda un LED cuando la temperatura supere 30°C.

## Código completo: Sistema multi-sensor

```c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_log.h"

#define POT_CHANNEL    ADC_CHANNEL_0
#define TEMP_CHANNEL   ADC_CHANNEL_1
#define LIGHT_CHANNEL  ADC_CHANNEL_2

static const char *TAG = "MULTI_SENSOR";

typedef struct {
    adc_oneshot_unit_handle_t adc_handle;
    adc_cali_handle_t cali_handle;
} sensor_context_t;

void init_adc(sensor_context_t *ctx)
{
    adc_oneshot_unit_init_cfg_t init_config = {.unit_id = ADC_UNIT_1};
    adc_oneshot_new_unit(&init_config, &ctx->adc_handle);
    
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_11,
    };
    
    adc_oneshot_config_channel(ctx->adc_handle, POT_CHANNEL, &config);
    adc_oneshot_config_channel(ctx->adc_handle, TEMP_CHANNEL, &config);
    adc_oneshot_config_channel(ctx->adc_handle, LIGHT_CHANNEL, &config);
    
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_12,
    };
    adc_cali_create_scheme_line_fitting(&cali_config, &ctx->cali_handle);
}

void app_main(void)
{
    sensor_context_t ctx;
    init_adc(&ctx);
    
    while (1) {
        int raw_pot, raw_temp, raw_light;
        int mv_pot, mv_temp, mv_light;
        
        // Leer todos los canales
        adc_oneshot_read(ctx.adc_handle, POT_CHANNEL, &raw_pot);
        adc_oneshot_read(ctx.adc_handle, TEMP_CHANNEL, &raw_temp);
        adc_oneshot_read(ctx.adc_handle, LIGHT_CHANNEL, &raw_light);
        
        // Convertir a voltaje
        adc_cali_raw_to_voltage(ctx.cali_handle, raw_pot, &mv_pot);
        adc_cali_raw_to_voltage(ctx.cali_handle, raw_temp, &mv_temp);
        adc_cali_raw_to_voltage(ctx.cali_handle, raw_light, &mv_light);
        
        // Calcular valores físicos
        float pot_percent = (mv_pot / 2500.0) * 100;
        float temperature = mv_temp / 10.0;  // LM35: 10mV/°C
        float light_percent = (mv_light / 2500.0) * 100;
        
        ESP_LOGI(TAG, "Pot: %.1f%% | Temp: %.1f°C | Luz: %.1f%%", 
                pot_percent, temperature, light_percent);
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

## Solución de problemas

| Problema | Solución |
|----------|----------|
| Lecturas muy ruidosas | Usar filtro mediana o promediado, capacitor 100nF en entrada |
| Lecturas siempre 0 o 4095 | Verificar rango de voltaje, ajustar atenuación |
| Lecturas inestables | Mejorar alimentación, agregar capacitor de desacople |
| Precisión baja | Usar calibración, promedio de muestras |

## Próximos pasos

- [Lab 4: Publicación Wi-Fi](./lab04-wifi.md)
- [Guía ADC y DAC](../guide/adc-dac.md)
- [Acondicionamiento de señal](../guide/signal-conditioning.md)

## Referencias

- [ESP32-C6 ADC](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/peripherals/adc_oneshot.html)
- [LM35 Datasheet](https://www.ti.com/lit/ds/symlink/lm35.pdf)
- [ADC Calibration](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/peripherals/adc_calibration.html)
