# Sensores y Transductores

Los sensores son dispositivos que detectan cambios en magnitudes físicas y los transductores convierten esas magnitudes en señales eléctricas procesables.

## Diferencia entre sensor y transductor

**Sensor**: Detecta un fenómeno físico
- Ejemplo: termistor detecta cambios de temperatura

**Transductor**: Convierte energía de una forma a otra
- Ejemplo: micrófono convierte sonido (energía mecánica) en voltaje (energía eléctrica)

En la práctica, ambos términos se usan de manera intercambiable.

## Tipos de sensores

### Por tipo de salida

#### 1. Sensores analógicos
Producen una señal continua proporcional a la magnitud medida.

| Sensor | Magnitud | Salida |
|--------|----------|--------|
| LM35 | Temperatura | 10 mV/°C |
| Fotoresistencia (LDR) | Luz | Resistencia variable |
| Potenciómetro | Posición | Voltaje variable |
| Acelerómetro analógico | Aceleración | Voltaje |

**Ventajas**: alta resolución, simple
**Desventajas**: susceptible a ruido, cables largos degradan señal

#### 2. Sensores digitales
Entregan datos digitales directamente (I²C, SPI, 1-Wire).

| Sensor | Magnitud | Interface |
|--------|----------|-----------|
| BME280 | Temperatura, presión, humedad | I²C/SPI |
| MPU6050 | Acelerómetro, giroscopio | I²C |
| DS18B20 | Temperatura | 1-Wire |
| MAX30102 | Oximetría (SpO₂) | I²C |

**Ventajas**: inmune a ruido, fácil integración, calibración interna
**Desventajas**: más costosos, protocolos de comunicación

### Por principio de funcionamiento

#### Resistivos
Cambian su resistencia en función de la magnitud medida.

- **Termistores (NTC/PTC)**: temperatura
- **LDR**: luz
- **FSR (Force Sensitive Resistor)**: fuerza/presión
- **Strain gauge**: deformación

**Circuito típico (divisor de voltaje)**:
```
VCC (3.3V) ──┬── R_fijo (10kΩ)
             │
             ├── Vout → ADC
             │
             └── Sensor_resistivo
             │
            GND
```

$$V_{out} = V_{CC} \cdot \frac{R_{sensor}}{R_{sensor} + R_{fijo}}$$

#### Capacitivos
Cambian su capacitancia.

- **Sensores de humedad**
- **Sensores táctiles capacitivos**
- **Acelerómetros MEMS**

#### Inductivos
Detectan campos magnéticos o metales.

- **Sensores de proximidad inductivos**
- **Encoders magnéticos**

#### Piezoeléctricos
Generan voltaje al aplicar presión/vibración.

- **Sensores de vibración**
- **Micrófonos piezoeléctricos**
- **Sensores de golpes/impactos**

#### Ópticos
Usan luz para detectar.

- **Fotodiodos**: miden intensidad lumínica
- **Infrarrojos (IR)**: detección de objetos
- **Sensores de color**: RGB/TCS34725

#### MEMS (Micro-Electro-Mechanical Systems)
Sistemas microelectromecánicos integrados en chips.

- **Acelerómetros**: MPU6050, ADXL345
- **Giroscopios**: MPU6050, L3GD20
- **Magnetómetros**: HMC5883L, QMC5883L
- **Sensores de presión**: BMP280, BME280

## Sensores comunes para ESP32-C6

### Temperatura

#### DHT11 / DHT22
- **Tipo**: Digital (protocolo propietario)
- **Temperatura**: DHT11 (0-50°C ±2°C), DHT22 (-40-80°C ±0.5°C)
- **Humedad**: DHT11 (20-80% ±5%), DHT22 (0-100% ±2%)
- **Pines**: VCC, GND, DATA
- **Ventajas**: económico, fácil de usar
- **Desventajas**: lento (1 muestra cada 2 segundos)

#### BME280
- **Tipo**: Digital I²C/SPI
- **Temperatura**: -40-85°C (±1°C)
- **Presión**: 300-1100 hPa (±1 hPa)
- **Humedad**: 0-100% (±3%)
- **Ventajas**: alta precisión, 3 sensores en uno
- **Aplicaciones**: estaciones meteorológicas, altímetros

#### DS18B20
- **Tipo**: Digital 1-Wire
- **Rango**: -55-125°C (±0.5°C)
- **Ventajas**: múltiples sensores en un solo bus, direccionamiento único
- **Aplicaciones**: medición de temperatura en líquidos

### Luz

#### BH1750
- **Tipo**: Digital I²C
- **Rango**: 1-65535 lux
- **Ventajas**: sensor de luz ambiental calibrado
- **Aplicaciones**: control automático de iluminación

#### LDR (Light Dependent Resistor)
- **Tipo**: Analógico resistivo
- **Rango**: variable (pocos Ω a MΩ)
- **Ventajas**: muy económico
- **Desventajas**: no calibrado, respuesta lenta

### Movimiento

#### MPU6050
- **Tipo**: Digital I²C
- **Acelerómetro**: ±2g, ±4g, ±8g, ±16g
- **Giroscopio**: ±250°/s, ±500°/s, ±1000°/s, ±2000°/s
- **Ventajas**: DMP (Digital Motion Processor) interno, económico
- **Aplicaciones**: drones, estabilización, detección de caídas

#### ADXL345
- **Tipo**: Digital I²C/SPI
- **Acelerómetro**: ±2g, ±4g, ±8g, ±16g
- **Resolución**: 13 bits
- **Ventajas**: alta resolución, bajo consumo

### Distancia

#### HC-SR04
- **Tipo**: Ultrasónico digital (pulsos)
- **Rango**: 2-400 cm
- **Precisión**: ±3mm
- **Pines**: VCC, GND, TRIG, ECHO
- **Funcionamiento**: 
  1. Enviar pulso 10µs en TRIG
  2. Medir duración pulso en ECHO
  3. $distancia = \frac{duracion \times 0.034}{2}$ (cm)

#### VL53L0X
- **Tipo**: Digital I²C (Time-of-Flight láser)
- **Rango**: 30-2000mm
- **Precisión**: ±3%
- **Ventajas**: alta precisión, no afectado por color del objeto

### Gas y calidad del aire

#### MQ-2
- **Tipo**: Analógico resistivo
- **Detecta**: LPG, propano, metano, humo
- **Requiere**: precalentamiento 24-48h
- **Salida**: analógica y digital (comparador)

#### CCS811
- **Tipo**: Digital I²C
- **Mide**: CO₂ equivalente (eCO₂), TVOC
- **Rango**: 400-8192 ppm CO₂
- **Aplicaciones**: calidad del aire interior

## Características importantes

### 1. Sensibilidad
Cambio en la salida por unidad de cambio en la entrada.
- Ejemplo: LM35 = 10mV/°C

### 2. Tiempo de respuesta
Tiempo que tarda el sensor en reaccionar a un cambio.
- DHT22: 2 segundos
- Termistor: milisegundos

### 3. Linealidad
Qué tan recta es la curva de respuesta del sensor.
- Lineal: salida proporcional a la entrada
- No lineal: requiere tabla de calibración

### 4. Histéresis
Diferencia en la lectura según se llegue desde arriba o desde abajo.

### 5. Rango de operación
- Temperatura: -40°C a +85°C (típico para electrónica)
- Humedad relativa: 0-100%

## Conexión de sensores

### Sensores analógicos resistivos

```
VCC (3.3V) ──┬── 10kΩ
             │
             ├── GPIO0 (ADC)
             │
             └── Sensor
             │
            GND
```

**Código ESP32-C6:**
```c
#include "driver/adc.h"

adc1_config_width(ADC_WIDTH_BIT_12);
adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);

int raw = adc1_get_raw(ADC1_CHANNEL_0);
float voltage = (raw / 4095.0) * 3.3;
```

### Sensores I²C

```
ESP32-C6          Sensor
SDA (GPIO6) ──── SDA
SCL (GPIO7) ──── SCL
3.3V ──────────── VCC
GND ───────────── GND
```

**Requiere pull-up resistors** (4.7kΩ - 10kΩ) en SDA y SCL.

### Sensores SPI

```
ESP32-C6          Sensor
MOSI ────────────  SDI/MOSI
MISO ────────────  SDO/MISO
SCK ─────────────  SCK
CS ──────────────  CS
3.3V ────────────  VCC
GND ─────────────  GND
```

## Calibración de sensores

### Calibración de un punto
Ajustar el offset:
```c
int offset = 0;

// Leer valor conocido (por ejemplo, 0V)
int zero_reading = adc1_get_raw(ADC1_CHANNEL_0);
offset = -zero_reading;

// En lecturas futuras
int corrected_value = adc1_get_raw(ADC1_CHANNEL_0) + offset;
```

### Calibración de dos puntos
Ajustar offset y ganancia:
```c
// Punto 1: valor_conocido_1, lectura_1
// Punto 2: valor_conocido_2, lectura_2

float gain = (valor_conocido_2 - valor_conocido_1) / 
             (lectura_2 - lectura_1);
float offset = valor_conocido_1 - gain * lectura_1;

// Conversión
float valor_real = gain * lectura + offset;
```

## Buenas prácticas

1. **Alimentación estable**: usar reguladores de voltaje, capacitores de desacople
2. **Cables cortos**: minimizar interferencia en señales analógicas
3. **Pull-ups en I²C**: 4.7kΩ típico, 2.2kΩ para buses rápidos
4. **Tiempo de estabilización**: esperar después de encender el sensor
5. **Promediado**: tomar múltiples muestras y promediar
6. **Calibración periódica**: algunos sensores derivan con el tiempo
7. **Datasheet**: siempre consultar el datasheet del fabricante

## Próximos pasos

- [Comunicación I²C](./i2c.md)
- [Comunicación SPI](./spi.md)
- [ADC y DAC](./adc-dac.md)
- [Lab 1: Sensores I²C](../examples/lab01-i2c-sensors.md)

## Referencias

- [ESP32-C6 GPIO Matrix](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/peripherals/gpio.html)
- [Adafruit Sensor Guide](https://learn.adafruit.com/sensors)
- [SparkFun Sensor Tutorials](https://learn.sparkfun.com/tutorials/tags/sensors)
