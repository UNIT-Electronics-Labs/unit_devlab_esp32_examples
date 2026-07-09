# Lab 1: Lectura de Sensores I²C

En esta práctica aprenderás a leer sensores digitales mediante el protocolo I²C.

## Objetivos

- Configurar el bus I²C en ESP32-C6
- Escanear dispositivos en el bus
- Identificar el sensor BMI270 en el bus QWIIC
- Mostrar datos en el monitor serial

## Materiales

- Placa Pulsar C6 (ESP32-C6)
- DevLab Multi Hub Shield
- BMI270 sensor inercial IMU 6DoF I²C/SPI
- Arnés QWIIC hembra-hembra o QWIIC a Dupont
- Cables Dupont
- Protoboard

## Conexiones

```
Pulsar C6 / Multi Hub    BMI270
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
3.3V                     VCC
GND                      GND
GPIO6 (SDA)              SDA
GPIO7 (SCL)              SCL
```

![Diagrama de conexión I²C](./img/lab01-i2c-diagram.png)

::: tip Resistencias Pull-up
Los módulos QWIIC normalmente ya incluyen pull-ups para I²C. Si usas Dupont directo, evita duplicar demasiadas resistencias pull-up en el mismo bus.
:::

## Código base Arduino

Este laboratorio usa `Wire` para escanear el bus y leer el registro de
identificación del BMI270 sin depender de una librería externa.

### Parte 1: Escaneo de dispositivos I²C

```cpp
#include <Wire.h>

const int SDA_PIN = 6;
const int SCL_PIN = 7;

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.println("Escaneando bus I2C...");
}

void loop() {
  byte count = 0;

  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.print("Dispositivo encontrado en 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      count++;
      delay(10);
    }
  }

  Serial.print("Total de dispositivos: ");
  Serial.println(count);
  delay(5000);
}
```

### Parte 2: Identificación del BMI270

```cpp
#include <Wire.h>

const int SDA_PIN = 6;
const int SCL_PIN = 7;
const byte BMI270_ADDR = 0x68;  // Algunos módulos usan 0x69
const byte BMI270_CHIP_ID_REG = 0x00;

bool readRegister(byte address, byte reg, byte& value) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) {
    return false;
  }

  if (Wire.requestFrom(address, (byte)1) != 1) {
    return false;
  }

  value = Wire.read();
  return true;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  byte chipId = 0;
  if (readRegister(BMI270_ADDR, BMI270_CHIP_ID_REG, chipId)) {
    Serial.print("BMI270 chip ID: 0x");
    Serial.println(chipId, HEX);
  } else {
    Serial.println("No se pudo leer BMI270 en 0x68. Prueba direccion 0x69.");
  }
}

void loop() {
  delay(1000);
}
```

## Referencia avanzada ESP-IDF

Si necesitas portar esta practica a ESP-IDF, usa el driver I2C nativo y configura el BMI270 desde sus registros. Para el curso base, usa el sketch Arduino anterior con el material disponible.

## Ejercicios

### Ejercicio 1: Calibración
Lee el datasheet del BMI270 e identifica qué registros se requieren para configurar acelerómetro y giroscopio.

### Ejercicio 2: Múltiples sensores
Conecta el BMI270 y el OLED SSD1306 en el mismo bus I²C y verifica que el scanner detecte ambos dispositivos.

### Ejercicio 3: Promediado
Implementa un filtro de promediado móvil de 10 muestras para suavizar las lecturas.

## Solución de problemas

| Problema | Solución |
|----------|----------|
| No se detecta el sensor | Verificar conexiones, dirección (0x76 vs 0x77), alimentación |
| Chip ID incorrecto | Verificar dirección 0x68/0x69 y alimentación del BMI270 |
| Lecturas erráticas | Agregar delay después de la inicialización, verificar pull-ups |
| ACK error | Verificar resistencias pull-up, reducir velocidad I²C |

## Próximos pasos

- [Lab 2: Pantalla OLED](./lab02-oled-display.md) - Visualizar los datos en pantalla
- [Guía I²C](../guide/i2c.md) - Profundizar en el protocolo
- [Sensores y transductores](../guide/sensors.md) - Más sobre sensores

## Referencias

- [BMI270 product page](https://www.bosch-sensortec.com/products/motion-sensors/imus/bmi270/)
- [ESP32-C6 I²C Driver](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/peripherals/i2c.html) - referencia avanzada
