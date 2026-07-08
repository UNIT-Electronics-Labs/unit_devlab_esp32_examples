# Control de WS2812

Aprovecha las tiras LED WS2812 con la placa PULSAR C6. Aprende a controlar LEDs RGB y crear efectos de iluminación usando MicroPython.

Esta sección describe cómo controlar LEDs WS2812 usando la placa PULSAR C6. La placa incluye un pin GPIO conectado al LED WS2812 integrado.

| PIN | GPIO ESP32C6 |
|-----|--------------|
| DIN | 8            |

Mapeo de pines para WS2812

<div id="figura-PULSAR  C6-one">

<figure>
<img src="/sphinx-static/WS1280_LED.jpg" class="align-center"
style="width:40.0%" alt="/sphinx-static/WS1280_LED.jpg" />
<figcaption>Tira LED WS2812</figcaption>
</figure>

</div>

## Ejemplo de código

A continuación se muestra un ejemplo para controlar LEDs WS2812 con la placa PULSAR C6.

### MicroPython

``` python
from machine import Pin
from neopixel import NeoPixel
np = NeoPixel(Pin(8), 1)
np[0] = (255, 128, 0) # set to red, full brightness

np.write()
```

### C++

``` c++
#include <Adafruit_NeoPixel.h>
#define PIN 8
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);
void setup() {
   strip.begin();
   strip.setPixelColor(0, 255, 128, 0); // set to red, full brightness
   strip.show();
}
```

### esp-idf

``` c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rmt_tx.h"
#include "esp_err.h"

void app_main(void) {
   rmt_channel_handle_t tx_channel = NULL;
   rmt_tx_channel_config_t tx_config = {
      .gpio_num = GPIO_NUM_8,
      .clk_src = RMT_CLK_SRC_DEFAULT,
      .resolution_hz = 10000000, // 10MHz resolution, 1 tick = 0.1us
      .mem_block_symbols = 64,
      .trans_queue_depth = 4,
      .flags.invert_out = false,
      .flags.with_dma = false,
   };
   ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_config, &tx_channel));
   ESP_ERROR_CHECK(rmt_enable(tx_channel));

   rmt_encoder_handle_t bytes_encoder = NULL;
   rmt_bytes_encoder_config_t bytes_encoder_config = {
      .bit0 = {.level0 = 1, .duration0 = 3, .level1 = 0, .duration1 = 9},  // 0: ~0.3us high, ~0.9us low
      .bit1 = {.level0 = 1, .duration0 = 9, .level1 = 0, .duration1 = 3},  // 1: ~0.9us high, ~0.3us low
      .flags.msb_first = true,
   };
   ESP_ERROR_CHECK(rmt_new_bytes_encoder(&bytes_encoder_config, &bytes_encoder));

   rmt_transmit_config_t tx_trans_config = {
      .loop_count = 0,
   };

   uint8_t r = 255, g = 0, b = 0;

   while (1) {
      if (r == 255 && g < 255 && b == 0) {
            g++;
      } else if (g == 255 && r > 0 && b == 0) {
            r--;
      } else if (g == 255 && b < 255 && r == 0) {
            b++;
      } else if (b == 255 && g > 0 && r == 0) {
            g--;
      } else if (b == 255 && r < 255 && g == 0) {
            r++;
      } else if (r == 255 && b > 0 && g == 0) {
            b--;
      }
      uint8_t color_data[3] = {g, r, b};

      // printf("%d %d %d\n",r,g,b);

      ESP_ERROR_CHECK(rmt_transmit(tx_channel, bytes_encoder, color_data, sizeof(color_data), &tx_trans_config));
      ESP_ERROR_CHECK(rmt_tx_wait_all_done(tx_channel, portMAX_DELAY));
      vTaskDelay(pdMS_TO_TICKS(10));
   }
}
```

<div class="tip">

<div class="title">

Consejo

</div>

Para más información sobre la librería NeoPixel, consulta la [documentación de NeoPixel](https://github.com/lvidarte/esp8266/wiki/MicroPython:-NeoPixels).

</div>
