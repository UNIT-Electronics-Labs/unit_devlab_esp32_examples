# Adquisición de Datos

La adquisición de datos (DAQ - Data Acquisition) es el proceso de medir fenómenos físicos del mundo real y convertirlos en señales digitales que un sistema embebido puede procesar.

## Conceptos fundamentales

### Sistema de adquisición de datos

Un sistema DAQ completo incluye:

```
Fenómeno físico → Sensor → Acondicionamiento → ADC → Procesador → Salida
```

1. **Fenómeno físico**: temperatura, presión, luz, movimiento, etc.
2. **Sensor/Transductor**: convierte la magnitud física en señal eléctrica
3. **Acondicionamiento de señal**: amplificación, filtrado, aislamiento
4. **ADC**: conversión analógica a digital
5. **Procesador**: ESP32-C6 procesa los datos
6. **Salida**: display, almacenamiento, transmisión (Wi-Fi/BLE)

### Tipos de señales

#### Señales analógicas
- **Continuas** en tiempo y amplitud
- Ejemplo: voltaje que varía entre 0V y 3.3V
- Requieren ADC para ser procesadas digitalmente

#### Señales digitales
- **Discretas** en tiempo y amplitud
- Valores típicos: HIGH (1) o LOW (0)
- Ejemplo: sensor de presencia, botón

### Parámetros importantes

#### 1. Rango de medición (Range)
Valores mínimo y máximo que puede medir el sistema.
- Ejemplo: termómetro de -40°C a +125°C

#### 2. Resolución
Cambio más pequeño que puede detectarse.
- ADC de 12 bits: $2^{12} = 4096$ niveles
- Si el rango es 0-3.3V: $\frac{3.3V}{4096} \approx 0.8mV$

#### 3. Exactitud (Accuracy)
Diferencia entre el valor medido y el valor real.
- Ejemplo: ±0.5°C

#### 4. Precisión (Precision)
Repetibilidad de la medición.
- Medidas consistentes, aunque no necesariamente exactas

#### 5. Frecuencia de muestreo
Número de muestras por segundo (samples per second - SPS o Hz).

**Teorema de Nyquist**: Para reconstruir correctamente una señal, la frecuencia de muestreo debe ser **al menos el doble** de la frecuencia máxima de la señal:

$$f_{muestreo} \geq 2 \cdot f_{señal\_max}$$

Ejemplo:
- Señal de audio: hasta 20 kHz → muestrear a ≥40 kHz
- ECG (electrocardiograma): hasta 100 Hz → muestrear a ≥200 Hz

## Modos de adquisición

### 1. Muestreo único (Single-shot)
Tomar una muestra cuando se necesite.
```c
int adc_value = adc1_get_raw(ADC1_CHANNEL_0);
```

**Ventajas**: bajo consumo de energía
**Desventajas**: puede perder eventos rápidos

### 2. Muestreo continuo
Tomar muestras a intervalos regulares.
```c
while (1) {
    int value = adc1_get_raw(ADC1_CHANNEL_0);
    vTaskDelay(pdMS_TO_TICKS(100)); // Cada 100ms
}
```

**Ventajas**: captura eventos dinámicos
**Desventajas**: mayor consumo de energía

### 3. Muestreo por eventos
Tomar muestras solo cuando ocurre un evento (umbral, interrupción).

**Ventajas**: eficiente en energía y almacenamiento
**Desventajas**: puede perder datos entre eventos

## Cadena de medición

### Ejemplo: Medir temperatura ambiente

```
Temperatura → LM35 (sensor) → 0-1V analógico → Amplificador → 0-3.3V → ADC ESP32-C6 → Valor digital
```

1. **Sensor LM35**: 10mV/°C
   - 25°C → 250mV = 0.25V
   
2. **Amplificador no inversor**: ganancia 3.3x
   - 0.25V × 3.3 = 0.825V

3. **ADC 12 bits (0-3.3V)**:
   - Valor ADC: $\frac{0.825}{3.3} \times 4095 \approx 1023$

4. **Conversión a temperatura**:
   - $V = \frac{ADC\_value}{4095} \times 3.3$
   - $T = \frac{V}{3.3} \times 100°C$

## Características del ADC en ESP32-C6

| Parámetro | Valor |
|-----------|-------|
| Resolución | 12 bits (4096 niveles) |
| Canales | 7 canales (GPIO0-GPIO6) |
| Rango de voltaje | 0 - 2.5V (con atenuación configurable) |
| Velocidad máx. | ~83 kHz |
| Atenuaciones | 0dB, 2.5dB, 6dB, 11dB |

### Atenuación del ADC

La atenuación expande el rango de voltaje medible:

| Atenuación | Rango de voltaje |
|------------|------------------|
| 0 dB | 0 - 750 mV |
| 2.5 dB | 0 - 1050 mV |
| 6 dB | 0 - 1300 mV |
| 11 dB | 0 - 2500 mV |

## Fuentes de error

### 1. Ruido eléctrico
- **Solución**: filtrado, apantallamiento, twisted pair cables

### 2. Offset
- El ADC no lee exactamente 0V cuando la entrada es 0V
- **Solución**: calibración

### 3. Deriva térmica
- Las medidas varían con la temperatura
- **Solución**: usar sensores compensados en temperatura

### 4. Aliasing
- Cuando $f_{muestreo} < 2 \cdot f_{señal}$
- **Solución**: aumentar frecuencia de muestreo o usar filtro anti-aliasing

### 5. Cuantización
- Error inherente al redondear valores analógicos continuos a niveles discretos
- Error máximo: $\pm \frac{1}{2}$ LSB (Least Significant Bit)

## Procesamiento de datos

### Promediado (Averaging)
Reduce ruido aleatorio:
```c
int sum = 0;
for (int i = 0; i < 10; i++) {
    sum += adc1_get_raw(ADC1_CHANNEL_0);
}
int average = sum / 10;
```

### Filtro de mediana
Elimina picos esporádicos (glitches):
```c
int samples[5];
for (int i = 0; i < 5; i++) {
    samples[i] = adc1_get_raw(ADC1_CHANNEL_0);
}
// Ordenar y tomar el valor central
```

### Filtro paso bajo digital
Suaviza la señal:
```c
float alpha = 0.1;  // Factor de suavizado
float filtered = 0;

// En cada lectura
float new_value = adc1_get_raw(ADC1_CHANNEL_0);
filtered = alpha * new_value + (1 - alpha) * filtered;
```

## Aplicaciones típicas

1. **Monitoreo ambiental**: temperatura, humedad, calidad del aire
2. **Automatización**: control de procesos industriales
3. **Salud**: ECG, oxímetros, termómetros
4. **Energía**: medición de voltaje, corriente, potencia
5. **Agricultura**: humedad del suelo, luz, pH
6. **IoT**: datos de sensores enviados a la nube

## Ejemplo práctico con ESP32-C6

```c
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_log.h"

#define ADC_CHANNEL ADC1_CHANNEL_0  // GPIO0
#define ADC_ATTEN   ADC_ATTEN_DB_11
#define ADC_WIDTH   ADC_WIDTH_BIT_12

static const char *TAG = "DAQ";

void app_main(void)
{
    // Configurar ADC
    adc1_config_width(ADC_WIDTH);
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN);
    
    // Calibración del ADC
    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN, ADC_WIDTH, 
                            1100, &adc_chars);
    
    while (1) {
        // Leer valor crudo
        int adc_raw = adc1_get_raw(ADC_CHANNEL);
        
        // Convertir a voltaje (mV)
        uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_raw, &adc_chars);
        
        ESP_LOGI(TAG, "ADC: %d, Voltaje: %d mV", adc_raw, voltage);
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

## Próximos pasos

- [Sensores y transductores](./sensors.md)
- [ADC y DAC](./adc-dac.md)
- [Acondicionamiento de señal](./signal-conditioning.md)
- [Lab 3: ADC analógico](../examples/lab03-adc.md)

## Referencias

- [ESP32-C6 ADC Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/peripherals/adc_oneshot.html)
- National Instruments: [Data Acquisition Basics](https://www.ni.com/en-us/shop/data-acquisition.html)
