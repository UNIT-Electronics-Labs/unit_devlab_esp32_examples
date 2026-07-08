# Prácticas de Laboratorio

Este repositorio contiene prácticas de adquisición de datos e IoT para ESP32-C6 con la placa Pulsar C6.

## Requisitos

- Placa Pulsar C6 (ESP32-C6)
- ESP-IDF v5.1 o superior
- Sensores y componentes según cada laboratorio
- Protoboard y cables jumper

## Laboratorios disponibles

### [Lab 1: Sensores I²C](./lab01-i2c-sensors.md)
Aprende a leer sensores digitales mediante el protocolo I²C. Incluye:
- Configuración del bus I²C
- Scanner de dispositivos
- Lectura del sensor BME280 (temperatura, presión, humedad)
- Calibración y filtrado

**Duración estimada**: 2 horas  
**Dificultad**: ⭐⭐☆☆☆

---

### [Lab 2: Pantalla OLED](./lab02-oled-display.md)
Visualiza datos de sensores en una pantalla OLED SSD1306. Incluye:
- Configuración de pantalla I²C
- Dibujo de texto y gráficos
- Uso de LVGL para interfaces
- Dashboard de sensores en tiempo real

**Duración estimada**: 2-3 horas  
**Dificultad**: ⭐⭐⭐☆☆

---

### [Lab 3: ADC Analógico](./lab03-adc.md)
Convierte señales analógicas usando el ADC del ESP32-C6. Incluye:
- Configuración del ADC
- Lectura de potenciómetro, LM35, fotoresistencia
- Calibración para mayor precisión
- Filtrado de ruido (mediana, paso bajo)

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
- Control de LED desde smartphone
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

### 1. Crear proyecto

```bash
cd ~/esp
cp -r $IDF_PATH/examples/get-started/hello_world lab01
cd lab01
idf.py set-target esp32c6
```

### 2. Compilar

```bash
idf.py build
```

### 3. Flashear y monitorear

```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

**Atajos útiles en el monitor**:
- `Ctrl+]` - Salir del monitor
- `Ctrl+T` `Ctrl+H` - Ver comandos disponibles
- `Ctrl+T` `Ctrl+R` - Reiniciar ESP32-C6

### 4. Limpiar (si es necesario)

```bash
idf.py fullclean
```

## Componentes comunes

Estos componentes se usan en múltiples laboratorios:

| Componente | Usado en | Notas |
|------------|----------|-------|
| BME280 | Lab 1, 2, 4 | Sensor I²C temperatura/humedad/presión |
| OLED SSD1306 | Lab 2 | Pantalla 128x64 I²C |
| Potenciómetro 10kΩ | Lab 3 | Entrada analógica variable |
| LM35 | Lab 3 | Sensor temperatura analógico |
| LDR + Resistencia 10kΩ | Lab 3 | Fotoresistencia |
| LED | Lab 1, 5 | Indicador visual |
| Smartphone | Lab 4, 5 | Con apps nRF Connect, MQTT |

## Recursos adicionales

### Herramientas de software
- [ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/)
- [nRF Connect](https://www.nordicsemi.com/Products/Development-tools/nrf-connect-for-mobile) - App BLE
- [MQTT Explorer](http://mqtt-explorer.com/) - Cliente MQTT
- [ThingSpeak](https://thingspeak.com/) - Plataforma IoT gratuita

### Datasheets
- [ESP32-C6 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-c6_datasheet_en.pdf)
- [BME280 Datasheet](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf)
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
4. Ejecutar `idf.py flash` nuevamente

## Próximos pasos

Después de completar los laboratorios:
- Combina sensores de múltiples labs
- Crea tu propio proyecto IoT
- Explora [ESP-IDF examples](https://github.com/espressif/esp-idf/tree/master/examples)
- Consulta las [guías teóricas](../guide/esp32c6.md) para profundizar conceptos
