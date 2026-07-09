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
- TMP235 o DHT11 para mostrar una lectura real (opcional)
- Cables jumper
- Protoboard

## Conexiones

```
ESP32-C6 Pulsar C6    OLED SSD1306
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
3.3V                  VCC
GND                   GND
GPIO6 (SDA)           SDA
GPIO7 (SCL)           SCL
```

::: tip Conexión en paralelo
Como I²C es un bus, puedes conectar múltiples dispositivos en paralelo (SDA con SDA, SCL con SCL).
:::

## Instalación de bibliotecas Arduino

Instala estas librerías desde **Tools > Manage Libraries**:

- `Adafruit SSD1306`
- `Adafruit GFX Library`
- `DHT sensor library` si quieres mostrar temperatura y humedad del DHT11

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

Si necesitas portar esta practica a ESP-IDF, usa `esp_lcd` para el SSD1306. Para el curso base, usa el sketch Arduino anterior con la pantalla OLED disponible.

## Extensión: mostrar sensores disponibles

Para integrar el DHT11, instala `DHT sensor library` y actualiza el contenido antes de `display.display()`:

```cpp
float temperature = dht.readTemperature();
float humidity = dht.readHumidity();

display.clearDisplay();
display.setCursor(0, 0);
display.println("Pulsar C6");
display.print("Temp: ");
display.print(temperature, 1);
display.println(" C");
display.print("Hum: ");
display.print(humidity, 1);
display.println(" %");
display.display();
```

Para TMP235, lee el valor analogico como en el Lab 3 y muestra la temperatura calculada en la pantalla.

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

## Referencias

- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
- [SSD1306 Datasheet](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)
