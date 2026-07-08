# Conversión analógica a digital

Aprende a leer valores de sensores analógicos usando el módulo ADC de la placa **PULSAR C6** con ESP32-C6. Esta sección cubre los fundamentos de entrada analógica y técnicas de conversión.

## Definición de ADC

La conversión analógica a digital (ADC) convierte señales analógicas en valores digitales. El ESP32-C6 incluye múltiples canales ADC, lo que permite leer voltajes analógicos y convertirlos en valores digitales. A continuación se muestran los detalles para usar estos pines en operaciones ADC.

### Cuantificación y codificación de señales analógicas

Las señales analógicas son continuas y pueden tomar cualquier valor dentro de un rango. Las señales digitales, en cambio, son discretas y sólo pueden tomar valores específicos. Convertir una señal analógica a digital implica dos pasos: cuantificación y codificación.

- **Cuantificación**: consiste en dividir la señal analógica en niveles discretos. La cantidad de niveles determina la resolución del ADC. Por ejemplo, un ADC de 12 bits puede dividir la señal en 4096 niveles.
- **Codificación**: consiste en asignar un código digital a cada nivel cuantificado. Ese código representa el valor de la señal analógica en ese nivel.

## Mapeo de pines ADC

La siguiente tabla muestra la distribución de pines ADC en la placa **PULSAR C6** y sus GPIO correspondientes en el ESP32-C6.

| Pin Number | **PULSAR C6** | ESP32-C6 |
|------------|---------------|----------|
| 1          | A0/D14        | GPIO0    |
| 2          | A1/D15        | GPIO1    |
| 3          | A2/D16        | GPIO3    |
| 4          | A3/D17        | GPIO4    |
| 5          | A4/D18        | GPIO22   |
| 6          | A5/D19        | GPIO23   |
| 7          | A7            | GPIO5    |

Mapeo de pines ADC

## Clase ADC

La clase `machine.ADC` se utiliza para crear objetos ADC que interactúan con los pines analógicos.

<div class="machine.ADC(pin)">

El constructor de la clase ADC recibe un único argumento: el número de pin.

</div>

## Definición de ejemplo

Para definir y usar un objeto ADC, sigue este ejemplo:

### MicroPython

``` python
import machine
adc = machine.ADC(0)  # Initialize ADC on pin A0
```

### C++

``` cpp
#define ADC0 0 // GPIO0 for A0
```

## Lectura de valores

Para leer el valor analógico convertido a formato digital:

### MicroPython

``` python
adc_value = adc.read()  # Read the ADC value
print(adc_value)  # Print the ADC value
```

### C++

``` cpp
voltage = analogRead(ADC0);
```

## Example Code

El siguiente ejemplo lee continuamente un pin ADC e imprime los resultados:

### MicroPython

``` python
import machine
import time

# Setup
adc = machine.ADC(machine.Pin(0))  # Initialize pin GPIO0 for ADC

# Continuous reading
while True:
    adc_value = adc.read_u16()        # Read the ADC value
    print(f"ADC Reading: {adc_value:.2f}")  # Print the ADC value
    time.sleep(1)                     # Delay for 1 second   
```

### C++

``` cpp
const int adcPin = 0; // GPIO0 (A0)
int adcValue = 0;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); // Set resolution to 12-bit
  delay(1000);
}

void loop() {
  // Reading ADC value
  adcValue = analogRead(adcPin);
  Serial.println(adcValue);
  delay(500);
}
```

### esp-idf

``` c
#include <stdio.h>
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"

static const char *TAG = "ADC_MIN";

void app_main(void)
{
    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_cfg, &adc_handle));

    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,  // <- Usa el recomendado
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_2, &chan_cfg)); // GPIO2

    int adc_raw;
    while (1) {
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL_2, &adc_raw));
        ESP_LOGI(TAG, "Lectura ADC (GPIO2): %d", adc_raw);
        vTaskDelay(pdMS_TO_TICKS(1000));  // <- Necesitabas incluir FreeRTOS
    }
}
```

<div id="figure_adc">

<figure>
<img src="/sphinx-static/nanoc6/adc.png" class="align-center"
style="width:95.0%" alt="/sphinx-static/nanoc6/adc.png" />
<figcaption>Ejemplo de entrada ADC0 en la placa <strong>PULSAR C6</strong>.</figcaption>
</figure>

</div>
