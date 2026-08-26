<script setup>
import { withBase } from 'vitepress'
</script>

# Pines de entrada/salida de propósito general (GPIO)

Los pines GPIO (entrada/salida de propósito general) de la placa **PULSAR C6** se utilizan para conectar dispositivos externos al microcontrolador. Estos pines pueden configurarse como entrada o salida. En esta sección se muestra cómo trabajar con GPIO usando MicroPython y C++.

<div id="figura2-dualmcu-one">

<figure>
<img :src="withBase('/sphinx-static/nanoc6/top.png')" class="align-center"
style="width:60.0%" alt="top.png" />
<figcaption>Placa de desarrollo <strong>PULSAR C6</strong></figcaption>
</figure>

</div>

## Aclaración sobre los pines ADC

En la tarjeta **PULSAR C6**, las posiciones **A4 y A5 no funcionan como
entradas ADC**. La serigrafía se realizó antes de finalizar el diseño de la PCB
y conservó la nomenclatura del formato Arduino Nano. En el diseño final, estas
posiciones se destinaron al bus **I²C**:

- **A4 (SDA)** → GPIO22
- **A5 (SCL)** → GPIO23

Por lo tanto, aunque sus etiquetas comienzan con la letra `A`, **A4 y A5 no
corresponden a entradas analógicas en esta tarjeta**.

Los siete canales ADC del ESP32-C6 sí están disponibles; solamente se
reasignaron a las siguientes posiciones:

| Posición en la tarjeta | GPIO | Canal ADC |
|---|---|---|
| A0 | GPIO0 | ADC1_CHANNEL_0 |
| A1 | GPIO1 | ADC1_CHANNEL_1 |
| A2 | GPIO3 | ADC1_CHANNEL_3 |
| A3 | GPIO4 | ADC1_CHANNEL_4 |
| A7 | GPIO5 | ADC1_CHANNEL_5 |
| D12 (MISO) | GPIO2 | ADC1_CHANNEL_2 |
| D13 (SCK) | GPIO6 | ADC1_CHANNEL_6 |

No se perdió ningún canal ADC: se reasignaron sus posiciones para conservar
la compatibilidad mecánica con el formato Nano y dedicar A4/A5 al bus I²C.

Comencemos con un ejemplo simple: hacer parpadear un LED. El ejemplo demuestra cómo controlar pines GPIO en la placa **PULSAR C6** usando MicroPython y C++.

## Trabajo con LEDs en ESP32-C6

En esta sección aprenderás a controlar un LED con el microcontrolador. El LED se conecta a un pin GPIO y se controla su encendido y apagado con un programa sencillo.

### Ejemplo de parpadeo de LED

<div class="tip">

<div class="title">

Consejo

</div>

El siguiente ejemplo muestra cómo hacer parpadear un LED conectado al GPIO 6 de la placa **PULSAR C6**. El LED se enciende durante 1 segundo y se apaga durante 1 segundo de forma indefinida.

</div>

Elige la pestaña del entorno que estés utilizando:

::: code-group

```python [MicroPython]
import machine
import time

led = machine.Pin(6, machine.Pin.OUT)

def loop():
    while True:
        led.on()  # Turn the LED on
        time.sleep(1)  # Wait for 1 second
        led.off()  # Turn the LED off
        time.sleep(1)  # Wait for 1 second

loop()
```

```cpp [Arduino (C++)]
#define LED 6

// The setup function runs once when you press reset or power the board
void setup() {
    // Initialize digital pin LED as an output.
    pinMode(LED, OUTPUT);
}

// The loop function runs continuously
void loop() {
    digitalWrite(LED, HIGH);   // Turn the LED on (HIGH is the voltage level)
    delay(1000);              // Wait for 1 second
    digitalWrite(LED, LOW);   // Turn the LED off (LOW is the voltage level)
    delay(1000);              // Wait for 1 second
}
```

```c [ESP-IDF (Espressif C)]
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define BLINK_GPIO GPIO_NUM_6  // Puedes cambiarlo según tu hardware

void app_main(void)
{
    // Configura el GPIO como salida
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    while (1) {
        // Enciende el LED
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(500)); // 500 ms

        // Apaga el LED
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(pdMS_TO_TICKS(500)); // 500 ms
    }
}
```

:::
