# ESP32-C6 Pulsar Labs

Documentación y prácticas de laboratorio para **adquisición de datos e IoT** con ESP32-C6 y la placa Pulsar C6.

## 📚 Contenido del curso

### Conceptos teóricos

- **[Adquisición de datos](./docs/guide/data-acquisition.md)**: Fundamentos de DAQ, muestreo, resolución
- **[Sensores y transductores](./docs/guide/sensors.md)**: Tipos de sensores, conexión, calibración
- **[ADC y DAC](./docs/guide/adc-dac.md)**: Conversión analógica-digital en ESP32-C6
- **[Acondicionamiento de señal](./docs/guide/signal-conditioning.md)**: Amplificación, filtrado, protección
- **[Comunicación I²C](./docs/guide/i2c.md)**: Protocolo I²C para sensores digitales
- **[Comunicación SPI](./docs/guide/spi.md)**: Protocolo SPI de alta velocidad
- **[Wi-Fi y Bluetooth LE](./docs/guide/connectivity.md)**: Conectividad inalámbrica
- **[Plataformas IoT](./docs/guide/iot-platforms.md)**: ThingSpeak, MQTT, Firebase, AWS IoT

### Prácticas de laboratorio

1. **[Lab 1: Sensores I²C](./docs/examples/lab01-i2c-sensors.md)** - Lectura de sensores digitales (BME280)
2. **[Lab 2: Pantalla OLED](./docs/examples/lab02-oled-display.md)** - Visualización de datos en pantalla
3. **[Lab 3: ADC analógico](./docs/examples/lab03-adc.md)** - Conversión de señales analógicas
4. **[Lab 4: Publicación Wi-Fi](./docs/examples/lab04-wifi.md)** - Envío de datos a la nube
5. **[Lab 5: Bluetooth LE](./docs/examples/lab05-ble.md)** - Comunicación con dispositivos móviles

## 🚀 Inicio rápido

### Requisitos previos

- **Hardware**: Placa Pulsar C6 (ESP32-C6)
- **Software**: 
  - Python 3.8+
  - ESP-IDF v5.1+
  - Node.js 22+ (para documentación)

### Instalación de ESP-IDF

#### Linux (Ubuntu/Debian)

```bash
# Instalar dependencias
sudo apt-get update
sudo apt-get install git wget flex bison gperf python3 python3-pip python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0

# Clonar ESP-IDF
mkdir -p ~/esp
cd ~/esp
git clone --recursive https://github.com/espressif/esp-idf.git

# Instalar herramientas
cd ~/esp/esp-idf
./install.sh esp32c6

# Activar entorno (agregar a .bashrc)
. $HOME/esp/esp-idf/export.sh
```

#### Windows

1. Descargar [ESP-IDF Tools Installer](https://dl.espressif.com/dl/esp-idf/)
2. Ejecutar instalador y seleccionar ESP32-C6
3. Abrir "ESP-IDF PowerShell" desde el menú inicio

### Ver documentación

```bash
# Clonar este repositorio
git clone https://github.com/tu-usuario/unit_devlab_esp32_examples.git
cd unit_devlab_esp32_examples

# Instalar dependencias de documentación
npm install

# Ejecutar servidor de documentación
npm run docs:dev
```

Abre [http://localhost:5173](http://localhost:5173) en tu navegador.

### Compilar primer ejemplo

```bash
# Copiar ejemplo básico
cd ~/esp
cp -r $IDF_PATH/examples/get-started/hello_world .
cd hello_world

# Configurar target ESP32-C6
idf.py set-target esp32c6

# Compilar
idf.py build

# Flashear (ajusta el puerto)
idf.py -p /dev/ttyUSB0 flash monitor
```

## 🛠️ Estructura del repositorio

```
.
├── docs/
│   ├── index.md                    # Página principal
│   ├── guide/                      # Guías teóricas
│   │   ├── esp32c6.md              # Configuración ESP32-C6
│   │   ├── esp-idf.md              # Guía ESP-IDF
│   │   ├── data-acquisition.md     # Adquisición de datos
│   │   ├── sensors.md              # Sensores y transductores
│   │   ├── adc-dac.md              # ADC y DAC
│   │   ├── signal-conditioning.md  # Acondicionamiento de señal
│   │   ├── i2c.md                  # Comunicación I²C
│   │   ├── spi.md                  # Comunicación SPI
│   │   ├── connectivity.md         # Wi-Fi y Bluetooth LE
│   │   └── iot-platforms.md        # Plataformas IoT
│   └── examples/                   # Prácticas de laboratorio
│       ├── lab01-i2c-sensors.md
│       ├── lab02-oled-display.md
│       ├── lab03-adc.md
│       ├── lab04-wifi.md
│       └── lab05-ble.md
├── examples/                       # Código fuente (a crear)
└── package.json                    # Configuración npm
```

## 📝 Crear tu primer proyecto

### Proyecto desde plantilla

```bash
cd ~/esp
cp -r $IDF_PATH/examples/get-started/hello_world mi_proyecto
cd mi_proyecto

idf.py set-target esp32c6
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

### Proyecto desde cero

```bash
mkdir mi_proyecto
cd mi_proyecto

# Crear estructura
mkdir -p main
touch main/main.c main/CMakeLists.txt CMakeLists.txt
```

**CMakeLists.txt** (raíz):
```cmake
cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(mi_proyecto)
```

**main/CMakeLists.txt**:
```cmake
idf_component_register(SRCS "main.c"
                      INCLUDE_DIRS ".")
```

**main/main.c**:
```c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

void app_main(void)
{
    ESP_LOGI("MAIN", "¡Hola ESP32-C6!");
}
```

Compilar y flashear:
```bash
idf.py set-target esp32c6
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

## 🔧 Solución de problemas

### "No module named 'serial'"
```bash
pip install pyserial
```

### "Permission denied /dev/ttyUSB0" (Linux)
```bash
sudo usermod -a -G dialout $USER
# Cerrar sesión y volver a iniciar
```

### "Failed to connect to ESP32-C6"
1. Mantener presionado botón **BOOT**
2. Presionar y soltar **RESET**
3. Soltar **BOOT**
4. Ejecutar `idf.py flash` nuevamente

## 📖 Recursos adicionales

### Documentación oficial
- [ESP32-C6 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-c6_datasheet_en.pdf)
- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/)
- [ESP-IDF API Reference](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/index.html)

### Herramientas
- [nRF Connect](https://www.nordicsemi.com/Products/Development-tools/nrf-connect-for-mobile) - App BLE
- [MQTT Explorer](http://mqtt-explorer.com/) - Cliente MQTT
- [ThingSpeak](https://thingspeak.com/) - Plataforma IoT

### Comunidad
- [Espressif Forum](https://esp32.com/)
- [ESP32 Reddit](https://www.reddit.com/r/esp32/)
- [GitHub Discussions](https://github.com/espressif/esp-idf/discussions)

## 🤝 Contribuir

¿Encontraste un error o quieres agregar contenido?

1. Fork el repositorio
2. Crea una rama (`git checkout -b feature/mejora`)
3. Commit tus cambios (`git commit -am 'Agregar mejora'`)
4. Push a la rama (`git push origin feature/mejora`)
5. Abre un Pull Request

## 📄 Licencia

Este proyecto está bajo licencia MIT. Ver archivo `LICENSE` para más detalles.

## ✨ Créditos

Curso desarrollado para la asignatura de **Adquisición de Datos** con ESP32-C6 y Pulsar C6.

### Temas cubiertos
- ✅ Conceptos generales de adquisición de datos
- ✅ Sensores y transductores
- ✅ Conversión analógica y digital (ADC y DAC)
- ✅ Acondicionamiento de señal con op-amps y filtros
- ✅ Comunicación con sensores (I²C y SPI)
- ✅ Sistemas embebidos para adquisición de datos
- ✅ Conectividad Wi-Fi y Bluetooth LE
- ✅ Introducción a plataformas IoT

---

**¡Feliz codificación con ESP32-C6! 🚀**
