---
layout: home

hero:
  name: ESP32-C6 Pulsar Labs
  text: Adquisición de Datos e IoT con ESP32-C6
  tagline: Documentación práctica para aprender adquisición de datos, sensores, comunicación I²C/SPI, Wi-Fi, Bluetooth LE y plataformas IoT con la placa Pulsar C6.
  actions:
    - theme: brand
      text: Ver prácticas
      link: /examples/
    - theme: alt
      text: Configurar ESP32-C6
      link: /guide/esp32c6

features:
  - title: Adquisición de datos
    details: Aprende a trabajar con sensores, ADC/DAC, acondicionamiento de señal y conversión analógica-digital.
  - title: Comunicación de sensores
    details: Interfaces I²C y SPI para lectura de sensores digitales y visualización en pantallas OLED.
  - title: Conectividad IoT
    details: Wi-Fi y Bluetooth LE para publicación de datos y comunicación con dispositivos móviles y plataformas IoT.
  - title: Placa Pulsar C6
    details: Ejemplos específicos optimizados para la placa Pulsar C6 basada en ESP32-C6 con soporte RISC-V.
---

## Mapa Rápido

### Configuración
- [Configurar ESP32-C6 y Pulsar C6](./guide/esp32c6.md) ⭐
- [Guía de ESP-IDF](./guide/esp-idf.md)
- [Guía para Windows](./sphinx-migrated/01_setup.md)
- [Documentación migrada desde Sphinx](./sphinx-migrated/)

### Conceptos teóricos
- [Adquisición de datos](./guide/data-acquisition.md)
- [Sensores y transductores](./guide/sensors.md)
- [ADC y DAC](./guide/adc-dac.md)
- [Acondicionamiento de señal](./guide/signal-conditioning.md)
- [Comunicación I²C](./guide/i2c.md)
- [Comunicación SPI](./guide/spi.md)
- [Wi-Fi y Bluetooth LE](./guide/connectivity.md)
- [Plataformas IoT](./guide/iot-platforms.md)

### Prácticas de laboratorio
- [Lab 1: Sensores I²C](./examples/lab01-i2c-sensors.md)
- [Lab 2: Pantalla OLED](./examples/lab02-oled-display.md)
- [Lab 3: ADC analógico](./examples/lab03-adc.md)
- [Lab 4: Publicación Wi-Fi](./examples/lab04-wifi.md)
- [Lab 5: Bluetooth LE](./examples/lab05-ble.md)

## Inicio rápido

1. **Instalar dependencias**:
   ```bash
   npm install
   ```

2. **Ejecutar servidor de documentación**:
   ```bash
   npm run docs:dev
   ```

3. **Ver documentación**:
   Abre `http://localhost:5173` en tu navegador

## Estructura del curso

El curso está organizado en dos partes:

### 1. Fundamentos teóricos
- Conceptos de adquisición de datos
- Sensores y transductores
- ADC/DAC y acondicionamiento de señal
- Protocolos de comunicación (I²C, SPI)
- Conectividad inalámbrica (Wi-Fi, Bluetooth LE)
- Plataformas IoT

### 2. Prácticas de laboratorio
- **Lab 1**: Sensores digitales I²C
- **Lab 2**: Visualización en pantalla OLED
- **Lab 3**: Conversión analógica-digital
- **Lab 4**: Publicación de datos Wi-Fi
- **Lab 5**: Comunicación Bluetooth LE
