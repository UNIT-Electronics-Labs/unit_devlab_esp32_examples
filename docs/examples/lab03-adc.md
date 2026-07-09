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
- TMP235 sensor de temperatura analógico
- Cables jumper
- Protoboard

## Teoría: ADC en ESP32-C6

El ADC del ESP32-C6 tiene las siguientes características:
- **Resolución**: 12 bits (4096 niveles: 0-4095)
- **Canales**: 7 (GPIO0-GPIO6)
- **Rango**: 0-2.5V (con atenuación 11dB)

## Código base Arduino

Este sketch lee el TMP235 por GPIO0 y muestra el valor crudo, voltaje y
temperatura estimada en el monitor serial.

```cpp
const int TMP235_PIN = 0;
const float ADC_MAX = 4095.0;
const float VREF = 3.3;
const float TMP235_OFFSET_MV = 500.0;
const float TMP235_SCALE_MV_PER_C = 10.0;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
}

void loop() {
  int raw = analogRead(TMP235_PIN);
  float voltage = (raw / ADC_MAX) * VREF;
  float millivolts = voltage * 1000.0;
  float temperatureC = (millivolts - TMP235_OFFSET_MV) / TMP235_SCALE_MV_PER_C;

  Serial.print("ADC: ");
  Serial.print(raw);
  Serial.print("  Voltaje: ");
  Serial.print(voltage, 3);
  Serial.print(" V  Temperatura: ");
  Serial.print(temperatureC, 1);
  Serial.println(" C");

  delay(500);
}
```

Si tu módulo TMP235 usa otra función de transferencia, ajusta
`TMP235_OFFSET_MV` y `TMP235_SCALE_MV_PER_C` según su hoja de datos.

### Conexiones TMP235

```
TMP235
━━━━━━━━━━━━━━━━━━━━━━━━━━━━
VCC   ────── 3.3V
GND   ────── GND
VOUT  ────── GPIO0
```

## Referencia avanzada ESP-IDF

Si necesitas portar esta practica a ESP-IDF, usa `adc_oneshot` y aplica la misma conversion del TMP235. Para el curso base, usa el sketch Arduino anterior con el sensor disponible.

## Ejercicios

### Ejercicio 1: Batería
Mide el voltaje de una batería usando un divisor de voltaje. Calcula el porcentaje de carga.

### Ejercicio 2: Multi-canal
Combina la lectura del TMP235 con la pantalla OLED SSD1306 para mostrar la temperatura local.

### Ejercicio 3: Data logger
Guarda lecturas del ADC en NVS cada minuto y muestra historial.

### Ejercicio 4: Alertas
Implementa una alerta serial cuando la temperatura del TMP235 supere 30°C.

## Código completo: TMP235 con filtro

```cpp
const int TMP235_PIN = 0;
const float ADC_MAX = 4095.0;
const float VREF = 3.3;
const float TMP235_OFFSET_MV = 500.0;
const float TMP235_SCALE_MV_PER_C = 10.0;
const float ALPHA = 0.15;

float filteredTemperature = 0.0;

float readTmp235C() {
  int raw = analogRead(TMP235_PIN);
  float millivolts = (raw / ADC_MAX) * VREF * 1000.0;
  return (millivolts - TMP235_OFFSET_MV) / TMP235_SCALE_MV_PER_C;
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  filteredTemperature = readTmp235C();
}

void loop() {
  float current = readTmp235C();
  filteredTemperature = (ALPHA * current) + ((1.0 - ALPHA) * filteredTemperature);

  Serial.print("TMP235: ");
  Serial.print(current, 1);
  Serial.print(" C  Filtrado: ");
  Serial.print(filteredTemperature, 1);
  Serial.println(" C");

  if (filteredTemperature > 30.0) {
    Serial.println("ALERTA: temperatura alta");
  }

  delay(1000);
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

- [Arduino analogRead](https://docs.arduino.cc/language-reference/en/functions/analog-io/analogRead/)
- [ESP32-C6 ADC](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/peripherals/adc_oneshot.html) - referencia avanzada
- [ADC Calibration](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/peripherals/adc_calibration.html) - referencia avanzada
