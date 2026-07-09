# Placa de desarrollo PULSAR C6

## Introducción

Esta guía te ayudará a comenzar con la placa de desarrollo **PULSAR C6**. La **PULSAR C6** está basada en el microcontrolador ESP32C6 y está diseñada para prototipado y desarrollo de aplicaciones IoT. La placa incluye varias interfaces, como GPIO, I2C, SPI, UART y otras, además de soporte integrado para Wi-Fi y Bluetooth.

> Placa de desarrollo **PULSAR C6**

## Microcontrolador ESP32C6

El ESP32-C6 es un microcontrolador versátil de consumo ultra bajo, con conectividad avanzada y periféricos integrados. Es adecuado para una amplia variedad de aplicaciones IoT y sistemas embebidos.

<div style="text-align: center;">
   <button style="background-color: #87cefa; color: white; border: none; padding: 10px 20px;" onclick="window.open('https://www.espressif.com/sites/default/files/documentation/esp32-c6_datasheet_en.pdf', '_blank')">Hoja de datos ESP32C6</button>
</div>

### Arquitectura

- **Arquitectura de CPU**:
  - Basada en la **ISA RISC-V de 32 bits**.
  - Incluye:
    - Una **CPU de alto rendimiento (HP)** de hasta **160 MHz** con pipeline de 4 etapas.
    - Una **CPU de bajo consumo (LP)** de hasta **20 MHz** para tareas energéticamente eficientes.
- **Memoria**:
  - **320 KB de ROM** para arranque y funciones esenciales.
  - **512 KB de SRAM de alto rendimiento** (HP SRAM) para datos e instrucciones.
  - **16 KB de SRAM de bajo consumo** (LP SRAM), con retención de datos durante modos de suspensión.
  - Soporte opcional para flash y SRAM externas mediante SPI, Dual SPI, Quad SPI y QPI.
- **Características de seguridad**:
  - Arranque seguro y cifrado de memoria.
  - Aceleradores criptográficos por hardware para AES, RSA, SHA, ECC y HMAC.
  - Soporte para entorno de ejecución confiable (TEE).
- **Capacidades inalámbricas**:
  - **Wi-Fi 6 (2.4 GHz)**, Bluetooth 5.3, Zigbee y Thread (802.15.4) para opciones de conectividad versátiles.
  - Coexistencia integrada para operación simultánea de Wi-Fi, Bluetooth y 802.15.4.

### Características generales

- **GPIO y funcionalidad de E/S**:
  - Hasta **30 GPIO** (QFN40) o **22 GPIO** (QFN32).
  - Múltiples funciones de E/S mediante multiplexado de pines.
  - Soporte para configuraciones digitales y analógicas:
    - **ADC SAR de 12 bits** con hasta 7 canales.
    - **Sensor de temperatura** integrado.
- **Interfaces periféricas**:
  - Interfaces digitales:
    - Dos **UART**.
    - **I2C** e **I2S** para comunicación y procesamiento de audio.
    - **SPI** con múltiples modos para transferencia rápida de datos.
  - Controladores PWM:
    - **LED PWM** con hasta 6 canales.
    - **PWM para control de motor (MCPWM)** para control de precisión.
  - **Contador de pulsos** para medición de frecuencia y señales.
  - **Controlador USB Serial/JTAG** para depuración y comunicación serie.
- **Temporizadores**:
  - **Temporizador de sistema de 52 bits** para medición precisa de tiempo.
  - Dos **temporizadores de propósito general de 54 bits**.
  - Múltiples **watchdogs digitales** para confiabilidad.

### Gestión de energía

- Soporta cuatro modos de energía para optimizar el consumo:
  - **Active**, **Modem-sleep**, **Light-sleep**, and **Deep-sleep**.
- Consumo ultra bajo en **modo Deep-sleep** (7 µA).
- Conserva memoria y funciones críticas en modos de bajo consumo.

### Seguridad y aceleración por hardware

- **Controlador DMA general** para transferencias de datos eficientes.
- Aceleradores criptográficos por hardware integrados:
  - **AES**, **RSA**, **SHA**, and **ECC**.
- Arranque seguro y cifrado de flash para integridad del sistema.

### Aplicaciones

El ESP32-C6 es ideal para diversas aplicaciones, incluyendo:

- Dispositivos de hogar inteligente.
- Automatización industrial.
- Concentradores de sensores IoT y registradores de datos.
- Electrónica de consumo y otras aplicaciones.

### Soporte de desarrollo

- Totalmente compatible con **ESP-IDF** de Espressif (IoT Development Framework) para desarrollo profesional.
- Soporte para **Arduino IDE** para aficionados y tareas de programación más simples.
- Compatibilidad con SDK de terceros para integrarse en distintos flujos de trabajo.

### Dimensiones físicas

- **Formato compacto** adecuado para aplicaciones embebidas.
- Disponible en encapsulados QFN40 (5×5 mm) y QFN32 (5×5 mm), lo que ofrece versatilidad para distintos diseños.

<div class="caution">

<div class="title">

Precaución

</div>

Estas son especificaciones generales; según el fabricante y el módulo ESP32-C6 específico, pueden existir diferencias en características o capacidades adicionales.

</div>
