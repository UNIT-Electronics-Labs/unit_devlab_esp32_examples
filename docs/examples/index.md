# Prácticas de Laboratorio

Este repositorio contiene prácticas de adquisición de datos e IoT para ESP32-C6 con la placa Pulsar C6.

## Requisitos

- Placa Pulsar C6 (ESP32-C6)
- Arduino IDE 2.x o Arduino CLI
- Paquete de placas ESP32/UNIT Electronics compatible con ESP32-C6
- Sensores y componentes según cada laboratorio
- Protoboard y cables jumper

## Laboratorios disponibles

### [Lab 1: Sensores I²C](./lab01-i2c-sensors.md)
Aprende a leer sensores digitales mediante el protocolo I²C. Incluye:
- Configuración del bus I²C
- Scanner de dispositivos
- Identificación del módulo BMI270 por I²C
- Lectura base para integrar sensores QWIIC

**Duración estimada**: 2 horas  
**Dificultad**: ⭐⭐☆☆☆

---

### [Lab 2: Pantalla OLED](./lab02-oled-display.md)
Visualiza datos de sensores en una pantalla OLED SSD1306. Incluye:
- Configuración de pantalla I²C
- Dibujo de texto y gráficos
- Dashboard simple con datos del TMP235 o DHT11
- Integración con el bus QWIIC

**Duración estimada**: 2-3 horas  
**Dificultad**: ⭐⭐⭐☆☆

---

### [Lab 3: ADC Analógico](./lab03-adc.md)
Convierte señales analógicas usando el ADC del ESP32-C6. Incluye:
- Configuración del ADC
- Lectura del sensor TMP235
- Calibración para mayor precisión
- Filtrado de ruido y conversión a temperatura

**Duración estimada**: 2-3 horas  
**Dificultad**: ⭐⭐⭐☆☆

---

### [Lab 4: Publicación Wi-Fi](./lab04-wifi.md)
Conecta a Wi-Fi y publica datos a plataformas IoT. Incluye:
- Conexión a red Wi-Fi
- Peticiones HTTP GET/POST
- MQTT con HiveMQ
- Integración con ThingSpeak
- Servidor web simple

**Duración estimada**: 3-4 horas  
**Dificultad**: ⭐⭐⭐⭐☆

---

### [Lab 5: Bluetooth LE](./lab05-ble.md)
Comunícate con dispositivos móviles mediante Bluetooth LE. Incluye:
- iBeacon advertising
- Servidor GATT (lectura/escritura)
- Notificaciones BLE
- Lectura y notificación del sensor táctil TTP223B
- Bajo consumo de energía

**Duración estimada**: 3-4 horas  
**Dificultad**: ⭐⭐⭐⭐☆

---

## Progresión sugerida

Para aprovechar al máximo las prácticas, se recomienda completarlas en orden:

1. **Lab 1** → Fundamentos de comunicación I²C
2. **Lab 2** → Visualización de datos (combina Lab 1)
3. **Lab 3** → Adquisición analógica
4. **Lab 4** → Conectividad IoT con Wi-Fi
5. **Lab 5** → Comunicación móvil con BLE

## Flujo de trabajo recomendado

El flujo principal de estos laboratorios es Arduino. ESP-IDF queda como ruta
avanzada para revisar APIs de bajo nivel o portar proyectos profesionales.

### 1. Preparar Arduino IDE

1. Instala Arduino IDE 2.x.
2. Agrega el paquete de placas ESP32/UNIT Electronics desde **Archivo > Preferencias > Gestor de URLs adicionales de tarjetas**.
3. Instala el paquete desde **Herramientas > Placa > Gestor de placas**.
4. Selecciona la placa Pulsar C6 o una placa ESP32-C6 compatible.

### 2. Crear sketch

```cpp
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Pulsar C6 lista");
}

void loop() {
  delay(1000);
}
```

### 3. Verificar y cargar

- Usa **Programa > Verificar/Compilar** para compilar.
- Usa **Programa > Cargar** para flashear la placa.
- Abre **Herramientas > Monitor serie** a `115200` baudios.

### 4. Instalar librerías de laboratorio

Instala librerías desde **Herramientas > Administrar bibliotecas** conforme las requiera cada
práctica, por ejemplo `Adafruit SSD1306`, `DHT sensor library`, `PubSubClient`
o clientes BLE compatibles.

## Material disponible

| Producto | SKU | Uso sugerido |
|----------|-----|--------------|
| DevLab: Multi Hub Shield | AR5437 | Base de conexión para prácticas |
| Cargador de Carga Rápida 40W US + Cable Tipo C | AR3448 | Alimentación y programación |
| Cables Dupont Largos 20cm HH | AR0002 | Conexiones en protoboard |
| Cables Dupont Largos 20cm MM | AR0004 | Conexiones en protoboard |
| Protoboard 400 puntos blanco | AR0070 | Montaje de circuitos |
| Arnés QWIIC hembra-hembra 4 pines | AR4561 | Conexión I²C/QWIIC |
| Arnés QWIIC Dupont hembra-macho 4 pines | AR4562 | Conexión I²C a protoboard |
| Display OLED blanco 128x64 0.96" I²C SSD1306 | AR1851 | Lab 2 |
| TMP235 sensor de temperatura analógico | AR5066 | Lab 3 y Lab 4 |
| TTP223B sensor capacitivo touch | AR5065 | Lab 5 |
| BMI270 sensor inercial IMU 6DoF I²C/SPI | AR4733 | Lab 1 |
| DHT11 sensor de temperatura y humedad KY-015 | AR0033 | Lab 4 |

## Componentes comunes

Estos componentes se usan en múltiples laboratorios:

| Componente | Usado en | Notas |
|------------|----------|-------|
| BMI270 | Lab 1 | IMU 6DoF I²C/SPI |
| OLED SSD1306 | Lab 2 | Pantalla 128x64 I²C |
| TMP235 | Lab 3, 4 | Sensor de temperatura analógico |
| DHT11 KY-015 | Lab 4 | Sensor de temperatura y humedad |
| TTP223B | Lab 5 | Sensor táctil capacitivo |
| Multi Hub Shield | Todos | Base de conexión para módulos |
| Smartphone | Lab 4, 5 | Con apps nRF Connect, MQTT |

## Recursos adicionales

### Herramientas de software
- [Arduino IDE](https://www.arduino.cc/en/software)
- [Arduino ESP32 core](https://github.com/espressif/arduino-esp32)
- [nRF Connect](https://www.nordicsemi.com/Products/Development-tools/nrf-connect-for-mobile) - App BLE
- [MQTT Explorer](http://mqtt-explorer.com/) - Cliente MQTT
- [ThingSpeak](https://thingspeak.com/) - Plataforma IoT gratuita

### Datasheets
- [ESP32-C6 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-c6_datasheet_en.pdf)
- [SSD1306 Datasheet](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)

## Solución de problemas generales

### Error: "No module named 'serial'"
```bash
pip install pyserial
```

### Error: "Permission denied /dev/ttyUSB0"
```bash
sudo usermod -a -G dialout $USER
# Cerrar sesión y volver a iniciar
```

### Error: "Failed to connect"
1. Mantener presionado botón **BOOT**
2. Presionar y soltar botón **RESET**
3. Soltar botón **BOOT**
4. Ejecutar **Cargar** nuevamente desde Arduino IDE

## Próximos pasos

Después de completar los laboratorios:
- Combina sensores de múltiples labs
- Crea tu propio proyecto IoT
- Explora ejemplos de Arduino desde **File > Examples**
- Consulta las [guías teóricas](../guide/esp32c6.md) para profundizar conceptos
