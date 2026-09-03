# Lab 2: Visualización en Pantalla OLED

En esta práctica conectarás una pantalla OLED SSD1306 y un sensor de luz
ambiental TEMT6000 Serie ATOM al mismo bus I²C. La pantalla mostrará la
lectura ADC actual, su porcentaje de escala completa y una gráfica del
historial de iluminación en tiempo real.

## Objetivos

- Conectar dos dispositivos con direcciones diferentes al mismo bus I²C
- Verificar la identidad DDP del TEMT6000 en la dirección `0x20`
- Leer muestras de luz ambiental de 12 bits, entre 0 y 4095
- Mostrar el valor actual y una gráfica histórica en la OLED
- Comprobar la respuesta del sensor al cubrirlo e iluminarlo

## Materiales

- Placa Pulsar C6 (ESP32-C6)
- Pantalla OLED 128x64 SSD1306 (I²C)
- Sensor de luz ambiental TEMT6000 Serie ATOM (I²C)
- Hub Qwiic/I²C o protoboard para conectar ambos dispositivos en paralelo
- Cables Qwiic o cables jumper

## Conexiones

Conecta la OLED y el TEMT6000 en paralelo. Ambos comparten alimentación,
SDA y SCL, pero responden en direcciones I²C diferentes.

| Pulsar C6 | OLED SSD1306 | TEMT6000 Serie ATOM |
|---|---|---|
| 3.3 V | VCC | VCC |
| GND | GND | GND |
| GPIO6 (SDA) | SDA | SDA |
| GPIO7 (SCL) | SCL | SCL |

| Dispositivo | Dirección I²C |
|---|---|
| OLED SSD1306 | `0x3C` |
| TEMT6000 Serie ATOM | `0x20` |

::: tip Conexión en paralelo
Como I²C es un bus direccionado, la OLED y el sensor pueden compartir
`SDA` y `SCL`. Para esta práctica alimenta ambos módulos a 3.3 V y conecta
sus tierras en común.
:::

## Instalación de bibliotecas Arduino

Instala estas librerías desde **Herramientas > Administrar bibliotecas**:

- `Adafruit SSD1306`
- `Adafruit GFX Library`
- `DevLabDDP`

## Código Arduino: TEMT6000 con gráfica en OLED

```cpp
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DevLabDDP.h>

constexpr uint8_t SDA_PIN = 6U;
constexpr uint8_t SCL_PIN = 7U;
constexpr uint32_t I2C_FREQUENCY = 400000U;

constexpr uint8_t OLED_ADDRESS = 0x3C;
constexpr uint8_t TEMT6000_ADDRESS = 0x20;
constexpr uint16_t ADC_MAX = 4095U;
constexpr uint32_t SAMPLE_INTERVAL_MS = 50U;

constexpr int16_t SCREEN_WIDTH = 128;
constexpr int16_t SCREEN_HEIGHT = 64;
constexpr int8_t OLED_RESET = -1;

constexpr int16_t GRAPH_X = 0;
constexpr int16_t GRAPH_Y = 19;
constexpr int16_t GRAPH_WIDTH = 128;
constexpr int16_t GRAPH_HEIGHT = 45;
constexpr int16_t PLOT_LEFT = 1;
constexpr int16_t PLOT_TOP = 20;
constexpr int16_t PLOT_BOTTOM = 62;
constexpr size_t HISTORY_LENGTH = 126;

Adafruit_SSD1306 display(
    SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DevLabDDP::Master temt6000(
    Wire, DevLabDDP::DEVICE_TEMT6000);

uint16_t history[HISTORY_LENGTH] = {};
size_t historyCount = 0;
size_t writeIndex = 0;
bool sensorReady = false;
uint32_t nextSampleMs = 0U;

void addSample(uint16_t value) {
  history[writeIndex] = value;
  writeIndex = (writeIndex + 1U) % HISTORY_LENGTH;

  if (historyCount < HISTORY_LENGTH) {
    historyCount++;
  }
}

uint16_t sampleAt(size_t position) {
  const size_t oldest =
      (historyCount == HISTORY_LENGTH) ? writeIndex : 0U;
  return history[(oldest + position) % HISTORY_LENGTH];
}

int16_t sampleToY(uint16_t value) {
  return static_cast<int16_t>(map(
      value, 0, ADC_MAX, PLOT_BOTTOM, PLOT_TOP));
}

void drawGraph(uint16_t currentValue) {
  const uint8_t percent = static_cast<uint8_t>(
      (static_cast<uint32_t>(currentValue) * 100U) / ADC_MAX);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(F("TEMT6000 0x20"));
  display.setCursor(0, 9);
  display.print(F("ADC:"));
  display.print(currentValue);
  display.print(' ');
  display.print(percent);
  display.print('%');

  display.drawRect(
      GRAPH_X, GRAPH_Y, GRAPH_WIDTH, GRAPH_HEIGHT, SSD1306_WHITE);

  // Línea guía correspondiente al 50 % de la escala ADC.
  const int16_t middleY = sampleToY(ADC_MAX / 2U);
  for (int16_t x = PLOT_LEFT; x < SCREEN_WIDTH - 1; x += 4) {
    display.drawPixel(x, middleY, SSD1306_WHITE);
  }

  if (historyCount == 1U) {
    display.drawPixel(
        SCREEN_WIDTH - 2, sampleToY(sampleAt(0)), SSD1306_WHITE);
  } else if (historyCount > 1U) {
    const int16_t xOffset = static_cast<int16_t>(
        HISTORY_LENGTH - historyCount);

    for (size_t i = 1; i < historyCount; i++) {
      const int16_t x0 = PLOT_LEFT + xOffset +
                         static_cast<int16_t>(i - 1U);
      const int16_t x1 = x0 + 1;
      display.drawLine(
          x0,
          sampleToY(sampleAt(i - 1U)),
          x1,
          sampleToY(sampleAt(i)),
          SSD1306_WHITE);
    }
  }

  display.display();
}

void showSensorError() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("TEMT6000"));
  display.println(F("No detectado"));
  display.println(F("Revise direccion 0x20"));
  display.display();
}

void setup() {
  Serial.begin(115200);
  delay(500);

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(I2C_FREQUENCY);

  // El bus ya fue iniciado con los pines de la Pulsar C6.
  if (!display.begin(
          SSD1306_SWITCHCAPVCC, OLED_ADDRESS, true, false)) {
    Serial.println(F("ERROR: OLED SSD1306 no detectada en 0x3C"));
    for (;;) {
      delay(10);
    }
  }

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false);

  DevLabDDP::DeviceInfo info;
  sensorReady = temt6000.matchesExpectedDevice(
      TEMT6000_ADDRESS, &info);

  if (!sensorReady) {
    Serial.println(
        F("ERROR: 0x20 no es un TEMT6000 DDP (ID 0x0102)"));
    showSensorError();
    return;
  }

  DevLabDDP::printDeviceInfo(
      Serial,
      TEMT6000_ADDRESS,
      info,
      DevLabDDP::DEVICE_TEMT6000);
  Serial.println(F("adc0_raw"));
}

void loop() {
  if (!sensorReady) {
    delay(1000);
    return;
  }

  const uint32_t now = millis();
  if (static_cast<int32_t>(now - nextSampleMs) < 0) {
    delay(1);
    return;
  }

  // Conserva un periodo de muestreo estable aunque la lectura o el dibujo
  // consuman algunos milisegundos.
  nextSampleMs += SAMPLE_INTERVAL_MS;
  if (static_cast<int32_t>(now - nextSampleMs) >= 0) {
    nextSampleMs = now + SAMPLE_INTERVAL_MS;
  }

  uint16_t raw;
  if (temt6000.readAdc(TEMT6000_ADDRESS, 0, raw)) {
    addSample(raw);
    drawGraph(raw);
    Serial.println(raw);
  } else {
    Serial.println(F("ERR"));
  }
}
```

## Referencia avanzada ESP-IDF

Si necesitas portar esta práctica a ESP-IDF, usa `esp_lcd` para la OLED
SSD1306 e implementa las transacciones DDP del TEMT6000 sobre el
controlador I²C. Para el curso base, usa el sketch Arduino anterior.

## Funcionamiento de la gráfica

La pantalla se divide en dos áreas:

- En la parte superior aparece el valor ADC actual y su porcentaje
  respecto de la escala completa.
- En la parte inferior aparece el historial de las últimas 126 muestras.
- La escala vertical permanece fija entre 0 y 4095 para que los cambios
  observados sean comparables.
- La línea punteada representa el 50 % de la escala ADC.
- Se toma y se muestra una muestra cada 50 ms, equivalente a 20 muestras
  por segundo.
- Al almacenar 126 muestras, la gráfica representa aproximadamente los
  últimos 6.3 segundos.

::: warning Lecturas relativas
El resultado es un código ADC de 12 bits que representa un nivel relativo
de iluminación. No debe interpretarse directamente como lux sin una
calibración óptica contra un instrumento de referencia.
:::

## Prueba de funcionamiento

1. Carga el sketch y abre el monitor serial a 115200 baudios.
2. Comprueba que se reporte un dispositivo TEMT6000 con ID `0x0102` en
   la dirección `0x20`.
3. Cubre el TEMT6000 y observa cómo descienden el valor y la gráfica.
4. Ilumina gradualmente el sensor y verifica que la curva ascienda.
5. Alterna entre sombra y luz para producir transiciones visibles en el
   historial.

El ejercicio demuestra que la OLED y el TEMT6000 pueden operar al mismo
tiempo sobre el mismo bus I²C gracias a que utilizan direcciones distintas.

## Resultado esperado

- La OLED se detecta en `0x3C`.
- El TEMT6000 se identifica en `0x20` como dispositivo DDP `0x0102`.
- El monitor serial recibe valores entre 0 y 4095.
- La lectura numérica y la curva de la OLED responden a los cambios de
  iluminación.

## Solución de problemas

| Problema | Solución |
|----------|----------|
| La OLED no enciende | Verifica alimentación, dirección `0x3C` y continuidad de SDA/SCL |
| Aparece `TEMT6000 No detectado` | Verifica la dirección `0x20`, la orientación del conector y la tierra común |
| El monitor serial muestra `ERR` | Revisa el cableado, la estabilidad de alimentación y posibles colisiones en el bus |
| La gráfica permanece cerca de cero | Retira cualquier objeto que cubra el sensor y prueba con una fuente de luz |
| La lectura permanece en 4095 | Reduce la iluminación directa para evitar saturar la escala ADC |
| La gráfica cambia, pero no coincide con lux | Realiza una calibración contra un luxómetro; la lectura mostrada es relativa |

## Próximos pasos

- [Lab 3: ADC analógico](./lab03-adc.md)
- [Guía I²C](../guide/i2c.md)

## Referencias

- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
- [DevLabDDP](https://github.com/UNIT-Electronics-Labs/unit_devlab_ddp_library)
- [Referencia técnica del TEMT6000 Serie ATOM](https://unit-electronics-mx.github.io/unit_devlab_temt6000_ambient_light_sensor/hardware/unit_product_reference_v_1_1_0_atom_temt6000_ambient_light_sensor.pdf)
- [SSD1306 Datasheet](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)
