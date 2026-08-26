<script setup>
import { withBase } from 'vitepress'
</script>

# Conversión analógica a digital

Aprende a leer valores de sensores analógicos usando el módulo ADC de la placa **PULSAR C6** con ESP32-C6. Esta sección cubre los fundamentos de entrada analógica y técnicas de conversión.

## Definición de ADC

La conversión analógica a digital (ADC) convierte señales analógicas en valores digitales. El ESP32-C6 incluye múltiples canales ADC, lo que permite leer voltajes analógicos y convertirlos en valores digitales. A continuación se muestran los detalles para usar estos pines en operaciones ADC.

### Cuantificación y codificación de señales analógicas

Las señales analógicas son continuas y pueden tomar cualquier valor dentro de un rango. Las señales digitales, en cambio, son discretas y sólo pueden tomar valores específicos. Convertir una señal analógica a digital implica dos pasos: cuantificación y codificación.

- **Cuantificación**: consiste en dividir la señal analógica en niveles discretos. La cantidad de niveles determina la resolución del ADC. Por ejemplo, un ADC de 12 bits puede dividir la señal en 4096 niveles.
- **Codificación**: consiste en asignar un código digital a cada nivel cuantificado. Ese código representa el valor de la señal analógica en ese nivel.

## Mapeo de pines ADC

La siguiente tabla muestra la distribución real de los siete canales ADC en
la placa **PULSAR C6**:

| Posición en la tarjeta | GPIO | Canal ADC |
|---|---|---|
| A0 | GPIO0 | ADC1_CHANNEL_0 |
| A1 | GPIO1 | ADC1_CHANNEL_1 |
| A2 | GPIO3 | ADC1_CHANNEL_3 |
| A3 | GPIO4 | ADC1_CHANNEL_4 |
| A7 | GPIO5 | ADC1_CHANNEL_5 |
| D12 (MISO) | GPIO2 | ADC1_CHANNEL_2 |
| D13 (SCK) | GPIO6 | ADC1_CHANNEL_6 |

::: warning A4 y A5 no son entradas ADC

En la PULSAR C6, **A4 corresponde a SDA (GPIO22)** y **A5 corresponde a SCL
(GPIO23)**. Ambas posiciones están dedicadas al bus I²C. La nomenclatura A4/A5
se conservó por compatibilidad mecánica con el formato Arduino Nano, pero no
indica capacidad analógica en esta tarjeta.

:::

## Clase ADC

La clase `machine.ADC` se utiliza para crear objetos ADC que interactúan con los pines analógicos.

<div class="machine.ADC(pin)">

El constructor de la clase ADC recibe un único argumento: el número de pin.

</div>

## Definición de ejemplo

Para definir y usar un objeto ADC, sigue este ejemplo:

::: code-group

```python [MicroPython]
import machine
adc = machine.ADC(0)  # Initialize ADC on pin A0
```

```cpp [Arduino (C++)]
#define ADC0 0 // GPIO0 for A0
```

:::

## Lectura de valores

Para leer el valor analógico convertido a formato digital:

::: code-group

```python [MicroPython]
adc_value = adc.read()  # Read the ADC value
print(adc_value)  # Print the ADC value
```

```cpp [Arduino (C++)]
voltage = analogRead(ADC0);
```

:::

## Ejemplo completo

El siguiente contenedor reúne las implementaciones que leen continuamente un
pin ADC e imprimen los resultados:

::: code-group

```python [MicroPython]
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

```cpp [Arduino (C++)]
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

```c [ESP-IDF (Espressif C)]
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

:::

<div id="figure_adc">

<figure>
<img :src="withBase('/sphinx-static/nanoc6/adc.png')" class="align-center"
style="width:95.0%" alt="adc.png" />
<figcaption>Ejemplo de entrada ADC0 en la placa <strong>PULSAR C6</strong>.</figcaption>
</figure>

</div>
