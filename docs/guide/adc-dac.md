# ADC y DAC en ESP32-C6

El ESP32-C6 incluye conversores analógico-digital (ADC) y digital-analógico (DAC) para interfaz entre el mundo analógico y digital.

## ADC (Analog-to-Digital Converter)

### Características del ADC en ESP32-C6

| Característica | Especificación |
|----------------|----------------|
| Resolución | 12 bits (4096 niveles) |
| Unidades ADC | ADC1 solamente |
| Canales | 7 canales (GPIO0-GPIO6) |
| Velocidad máx. | ~83 kHz |
| Rango de voltaje | 0-2.5V (con atenuación) |
| Modos | One-shot, Continuous |

### Canales ADC disponibles

| Canal | GPIO | Uso recomendado |
|-------|------|-----------------|
| ADC1_CHANNEL_0 | GPIO0 | Entrada analógica |
| ADC1_CHANNEL_1 | GPIO1 | Entrada analógica |
| ADC1_CHANNEL_2 | GPIO2 | Entrada analógica |
| ADC1_CHANNEL_3 | GPIO3 | Entrada analógica |
| ADC1_CHANNEL_4 | GPIO4 | Entrada analógica |
| ADC1_CHANNEL_5 | GPIO5 | Entrada analógica |
| ADC1_CHANNEL_6 | GPIO6 | Entrada analógica (también I2C SDA) |

::: warning Conflictos con Wi-Fi
En ESP32 clásico, ADC2 no funciona con Wi-Fi activo. El ESP32-C6 solo tiene ADC1, evitando este problema.
:::

### Atenuación (Rango de voltaje)

La atenuación configura el rango de voltaje de entrada:

| Atenuación | Rango | Uso típico |
|------------|-------|------------|
| `ADC_ATTEN_DB_0` | 0 - 750 mV | Señales pequeñas |
| `ADC_ATTEN_DB_2_5` | 0 - 1050 mV | Baterías Li-Po (celda) |
| `ADC_ATTEN_DB_6` | 0 - 1300 mV | - |
| `ADC_ATTEN_DB_11` | 0 - 2500 mV | Señales 0-3.3V con divisor |

::: tip Divisor de voltaje
Para medir 0-3.3V con atenuación 11dB (0-2.5V):
```
Señal 3.3V ──┬── R1 (10kΩ)
             │
             ├── ADC (máx 2.5V)
             │
             └── R2 (22kΩ)
             │
            GND
```
$V_{ADC} = V_{in} \cdot \frac{R2}{R1 + R2} = 3.3V \cdot 0.69 = 2.27V$
:::

### Modo One-Shot (Lectura única)

Lee el ADC cuando lo necesites:

```c
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"

static const char *TAG = "ADC_ONESHOT";

void app_main(void)
{
    // Configuración
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc1_handle));
    
    // Configurar canal
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_11,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_0, &config));
    
    while (1) {
        int adc_raw;
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_0, &adc_raw));
        
        // Convertir a voltaje
        float voltage = (adc_raw / 4095.0) * 2.5; // Para ATTEN_DB_11
        
        ESP_LOGI(TAG, "ADC: %d, Voltaje: %.2f V", adc_raw, voltage);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

### Modo Continuo (DMA)

Adquiere muestras continuamente con DMA (Direct Memory Access) para alta velocidad:

```c
#include "esp_adc/adc_continuous.h"

#define SAMPLE_FREQ_HZ  20000  // 20 kHz
#define CONV_FRAME_SIZE 1024

static bool IRAM_ATTR adc_conv_done_callback(
    adc_continuous_handle_t handle,
    const adc_continuous_evt_data_t *edata,
    void *user_data)
{
    // Procesar datos
    return false;
}

void app_main(void)
{
    adc_continuous_handle_t handle = NULL;
    
    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = 1024,
        .conv_frame_size = CONV_FRAME_SIZE,
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &handle));
    
    adc_continuous_config_t dig_cfg = {
        .sample_freq_hz = SAMPLE_FREQ_HZ,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1,
    };
    
    adc_digi_pattern_config_t adc_pattern = {
        .atten = ADC_ATTEN_DB_11,
        .channel = ADC_CHANNEL_0,
        .unit = ADC_UNIT_1,
        .bit_width = ADC_BITWIDTH_12,
    };
    
    dig_cfg.pattern_num = 1;
    dig_cfg.adc_pattern = &adc_pattern;
    
    ESP_ERROR_CHECK(adc_continuous_config(handle, &dig_cfg));
    
    // Registrar callback
    adc_continuous_evt_cbs_t cbs = {
        .on_conv_done = adc_conv_done_callback,
    };
    ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(handle, &cbs, NULL));
    
    // Iniciar conversión
    ESP_ERROR_CHECK(adc_continuous_start(handle));
}
```

### Calibración del ADC

El ADC tiene variaciones de fabricación. La calibración mejora la precisión:

```c
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

adc_cali_handle_t adc_cali_handle = NULL;
adc_cali_line_fitting_config_t cali_config = {
    .unit_id = ADC_UNIT_1,
    .atten = ADC_ATTEN_DB_11,
    .bitwidth = ADC_BITWIDTH_12,
};

// Crear esquema de calibración
esp_err_t ret = adc_cali_create_scheme_line_fitting(&cali_config, &adc_cali_handle);

if (ret == ESP_OK) {
    // Leer ADC
    int adc_raw;
    adc_oneshot_read(adc1_handle, ADC_CHANNEL_0, &adc_raw);
    
    // Convertir a voltaje calibrado (mV)
    int voltage_mv;
    adc_cali_raw_to_voltage(adc_cali_handle, adc_raw, &voltage_mv);
    
    ESP_LOGI(TAG, "Voltaje calibrado: %d mV", voltage_mv);
}
```

### Filtrado de ruido

#### 1. Oversampling (Sobremuestreo)
Tomar múltiples muestras y promediar:

```c
#define SAMPLES 64

int adc_read_filtered(adc_oneshot_unit_handle_t handle, adc_channel_t channel)
{
    int sum = 0;
    for (int i = 0; i < SAMPLES; i++) {
        int raw;
        adc_oneshot_read(handle, channel, &raw);
        sum += raw;
    }
    return sum / SAMPLES;
}
```

Aumenta la resolución efectiva:
- 4 muestras → +1 bit (13 bits efectivos)
- 16 muestras → +2 bits (14 bits efectivos)
- 64 muestras → +3 bits (15 bits efectivos)

#### 2. Filtro paso bajo (Low-pass filter)

```c
float alpha = 0.1;  // Factor de suavizado (0-1)
float filtered_value = 0;

// En cada lectura
int raw;
adc_oneshot_read(handle, channel, &raw);
filtered_value = alpha * raw + (1 - alpha) * filtered_value;
```

#### 3. Filtro de mediana

```c
int median_filter(int *samples, int n)
{
    // Ordenar samples (bubble sort simple)
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (samples[j] > samples[j+1]) {
                int temp = samples[j];
                samples[j] = samples[j+1];
                samples[j+1] = temp;
            }
        }
    }
    return samples[n/2];
}
```

## DAC (Digital-to-Analog Converter)

::: warning DAC en ESP32-C6
El ESP32-C6 tiene soporte limitado de DAC. Para aplicaciones que requieren DAC externo de alta calidad, considera usar chips como MCP4725 (I²C) o MCP4822 (SPI).
:::

### DAC interno ESP32-C6

El ESP32-C6 incluye un DAC de **8 bits** en el GPIO16:

| Característica | Especificación |
|----------------|----------------|
| Resolución | 8 bits (256 niveles) |
| Canales | 1 (GPIO16) |
| Rango de salida | 0 - VDD (típicamente 3.3V) |
| Precisión | ±50 mV |

#### Ejemplo de uso

```c
#include "driver/dac_oneshot.h"
#include "esp_log.h"

static const char *TAG = "DAC";

void app_main(void)
{
    // Configurar DAC
    dac_oneshot_handle_t dac_handle;
    dac_oneshot_config_t dac_cfg = {
        .chan_id = DAC_CHAN_0,  // GPIO16
    };
    ESP_ERROR_CHECK(dac_oneshot_new_channel(&dac_cfg, &dac_handle));
    
    // Generar onda triangular
    for (int i = 0; i < 256; i++) {
        ESP_ERROR_CHECK(dac_oneshot_output_voltage(dac_handle, i));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
```

### DAC externo I²C: MCP4725

El MCP4725 es un DAC de **12 bits** con interface I²C:

```c
#include "driver/i2c.h"

#define MCP4725_ADDR 0x60
#define I2C_MASTER_NUM I2C_NUM_0

void mcp4725_set_voltage(uint16_t value)
{
    // value: 0-4095 (12 bits)
    uint8_t data[2];
    data[0] = (value >> 8) & 0x0F;  // 4 bits altos
    data[1] = value & 0xFF;          // 8 bits bajos
    
    i2c_master_write_to_device(I2C_MASTER_NUM, MCP4725_ADDR, 
                              data, 2, pdMS_TO_TICKS(1000));
}

// Generar onda senoidal
void app_main(void)
{
    // Configurar I2C primero...
    
    while (1) {
        for (int angle = 0; angle < 360; angle++) {
            float rad = angle * M_PI / 180.0;
            float sin_value = sin(rad);
            uint16_t dac_value = (uint16_t)((sin_value + 1.0) * 2047.5);
            
            mcp4725_set_voltage(dac_value);
            vTaskDelay(pdMS_TO_TICKS(5));
        }
    }
}
```

## Aplicaciones prácticas

### 1. Generador de señales

```c
// Generar onda cuadrada
void square_wave(dac_oneshot_handle_t dac, int freq_hz)
{
    int period_ms = 1000 / freq_hz;
    int half_period_ms = period_ms / 2;
    
    while (1) {
        dac_oneshot_output_voltage(dac, 255);  // HIGH
        vTaskDelay(pdMS_TO_TICKS(half_period_ms));
        dac_oneshot_output_voltage(dac, 0);    // LOW
        vTaskDelay(pdMS_TO_TICKS(half_period_ms));
    }
}
```

### 2. Control de volumen de audio

```c
// Ajustar nivel de audio (0-100%)
void set_volume(dac_oneshot_handle_t dac, uint8_t volume_percent)
{
    uint8_t dac_value = (volume_percent * 255) / 100;
    dac_oneshot_output_voltage(dac, dac_value);
}
```

### 3. Control de brillo LED (atenuación analógica)

```c
// Control de brillo (0-100%)
void set_led_brightness(dac_oneshot_handle_t dac, uint8_t brightness)
{
    uint8_t dac_value = (brightness * 255) / 100;
    dac_oneshot_output_voltage(dac, dac_value);
}
```

::: tip PWM como alternativa
Para control de brillo de LEDs, PWM es más común y eficiente que DAC. El ESP32-C6 tiene 6 canales LEDC PWM.
:::

### 4. Offset de voltaje variable

Generar un voltaje de referencia ajustable:

```c
// Generar 0V a 3.3V con resolución de ~13mV (8 bits)
void set_reference_voltage(dac_oneshot_handle_t dac, float voltage)
{
    if (voltage > 3.3) voltage = 3.3;
    if (voltage < 0) voltage = 0;
    
    uint8_t dac_value = (uint8_t)((voltage / 3.3) * 255);
    dac_oneshot_output_voltage(dac, dac_value);
}
```

## Comparación ADC vs DAC

| Característica | ADC | DAC |
|----------------|-----|-----|
| Función | Analógico → Digital | Digital → Analógico |
| Entrada/Salida | Voltaje analógico → Número digital | Número digital → Voltaje analógico |
| Aplicación | Leer sensores | Control, audio, señales |
| ESP32-C6 resolución | 12 bits (4096 niveles) | 8 bits (256 niveles) |
| Velocidad | ~83 kHz | Limitado por I2C/SPI en DAC externo |

## Buenas prácticas

### ADC
1. **Usa calibración** para mejor precisión
2. **Filtra ruido** con oversampling o filtros digitales
3. **Evita conflictos**: no uses GPIO6/GPIO7 si necesitas I²C
4. **Capacitor de desacople**: 100nF cerca del pin de entrada
5. **Rango adecuado**: selecciona la atenuación apropiada

### DAC
1. **Filtro paso bajo RC**: suaviza la salida escalonada
   ```
   DAC → R (1kΩ) ──┬── Salida
                    │
                   C (10µF)
                    │
                   GND
   ```
2. **Buffer de salida**: usa op-amp si necesitas alta corriente
3. **DAC externo**: para > 8 bits de resolución

## Próximos pasos

- [Acondicionamiento de señal](./signal-conditioning.md)
- [Lab 3: ADC analógico](../examples/lab03-adc.md)
- [Sensores analógicos](./sensors.md)

## Referencias

- [ESP32-C6 ADC Oneshot](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/peripherals/adc_oneshot.html)
- [ESP32-C6 ADC Continuous](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/peripherals/adc_continuous.html)
- [ESP32-C6 DAC](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/peripherals/dac.html)
