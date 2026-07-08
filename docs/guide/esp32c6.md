# Configurar ESP32-C6 y Pulsar C6

Esta guía te ayudará a configurar tu entorno de desarrollo para trabajar con ESP32-C6 y la placa Pulsar C6.

## ¿Qué es ESP32-C6?

El ESP32-C6 es un SoC (System on Chip) de Espressif que integra:

- **Procesador RISC-V de 32 bits** hasta 160 MHz
- **Wi-Fi 6 (802.11ax)** y **Bluetooth 5.3 (LE)**
- **12 canales ADC de 12 bits** (SAR)
- **1 canal DAC** de 8 bits
- **Periféricos de comunicación**: I²C, SPI, UART, I2S
- **30 GPIOs programables**
- **Seguridad**: motor criptográfico, arranque seguro, flash encryption

## Placa Pulsar C6

La Pulsar C6 es una placa de desarrollo que facilita el acceso a todas las capacidades del ESP32-C6, ideal para proyectos de adquisición de datos e IoT.

### Características principales

- Puerto USB-C para programación y alimentación
- Conversor USB-Serial integrado
- Botones de Reset y Boot
- LED indicador programable
- Acceso a todos los pines GPIO
- Conector para batería LiPo (opcional)
- Regulador de voltaje 3.3V

## Requisitos

### Software

- **Python 3.8 o superior**
- **Git**
- **ESP-IDF v5.1 o superior** (framework oficial de Espressif)
- **Editor de código** (VS Code recomendado)

### Hardware

- Placa Pulsar C6
- Cable USB-C
- (Opcional) Sensores, pantallas OLED, protoboard, cables jumper

## Instalación de ESP-IDF

### Linux (Ubuntu/Debian)

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

# Configurar variables de entorno (agregar al .bashrc o .zshrc)
. $HOME/esp/esp-idf/export.sh
```

::: tip Automatizar export.sh
Agrega el comando `. $HOME/esp/esp-idf/export.sh` a tu archivo `~/.bashrc` o `~/.zshrc` para tener ESP-IDF disponible en cada terminal nueva.
:::

### Windows

1. **Descargar el instalador oficial**:
   - Ir a [ESP-IDF Tools Installer](https://dl.espressif.com/dl/esp-idf/)
   - Descargar `esp-idf-tools-setup-x.x.x.exe`

2. **Ejecutar el instalador**:
   - Seleccionar ESP-IDF v5.1 o superior
   - Marcar ESP32-C6 en targets
   - Instalar en la ruta por defecto `C:\Espressif`

3. **Verificar instalación**:
   - Abrir "ESP-IDF PowerShell" o "ESP-IDF CMD" desde el menú inicio
   - Ejecutar: `idf.py --version`

### macOS

```bash
# Instalar Homebrew si no está instalado
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Instalar dependencias
brew install cmake ninja dfu-util python3

# Clonar ESP-IDF
mkdir -p ~/esp
cd ~/esp
git clone --recursive https://github.com/espressif/esp-idf.git

# Instalar herramientas
cd ~/esp/esp-idf
./install.sh esp32c6

# Configurar variables de entorno
. $HOME/esp/esp-idf/export.sh
```

## Verificar conexión con Pulsar C6

1. **Conectar la placa** al puerto USB
2. **Identificar el puerto serie**:

```bash
# Linux
ls /dev/ttyUSB* /dev/ttyACM*

# macOS
ls /dev/cu.*

# Windows (PowerShell)
[System.IO.Ports.SerialPort]::getportnames()
```

3. **Probar comunicación**:

```bash
idf.py --port /dev/ttyUSB0 monitor
# Presiona Ctrl+] para salir
```

## Crear tu primer proyecto

```bash
# Copiar proyecto de ejemplo
cd ~/esp
cp -r $IDF_PATH/examples/get-started/hello_world .
cd hello_world

# Configurar el proyecto
idf.py set-target esp32c6

# Compilar
idf.py build

# Flashear y monitorear
idf.py -p /dev/ttyUSB0 flash monitor
```

::: tip
Presiona `Ctrl+T` seguido de `Ctrl+H` en el monitor serial para ver todos los comandos disponibles.
:::

## Estructura de un proyecto ESP-IDF

```
mi_proyecto/
├── CMakeLists.txt          # Configuración de compilación
├── main/
│   ├── CMakeLists.txt      # Configuración del componente main
│   └── main.c              # Código principal
├── components/             # Componentes adicionales (opcional)
├── sdkconfig               # Configuración del proyecto (generado)
└── build/                  # Archivos compilados (generado)
```

## Configuración del proyecto

Puedes personalizar la configuración del proyecto con:

```bash
idf.py menuconfig
```

Opciones importantes para adquisición de datos:
- **Component config → ESP32C6-specific → CPU frequency**: velocidad del procesador
- **Component config → Driver configurations → ADC configuration**: configuración del ADC
- **Component config → Wi-Fi**: configuración Wi-Fi
- **Component config → Bluetooth**: configuración Bluetooth

## Drivers USB para Windows

En Windows, puede ser necesario instalar drivers USB manualmente:

1. Conectar la Pulsar C6
2. Abrir **Administrador de dispositivos**
3. Si aparece un dispositivo desconocido:
   - Descargar [Zadig](https://zadig.akeo.ie/)
   - Seleccionar el dispositivo USB-Serial
   - Instalar driver **WinUSB** o **USB Serial (CDC)**

## Solución de problemas

### Error: "No module named 'serial'"

```bash
pip install pyserial
```

### Error: "Permission denied /dev/ttyUSB0" (Linux)

```bash
sudo usermod -a -G dialout $USER
# Cerrar sesión y volver a iniciar
```

### Error: "A fatal error occurred: Failed to connect"

1. Mantener presionado el botón **BOOT**
2. Presionar y soltar el botón **RESET**
3. Soltar el botón **BOOT**
4. Ejecutar `idf.py flash` nuevamente

## Próximos pasos

- [Guía de ESP-IDF](./esp-idf.md) - Aprende a usar el framework
- [Lab 1: Sensores I²C](../examples/lab01-i2c-sensors.md) - Primera práctica
- [Adquisición de datos](./data-acquisition.md) - Conceptos teóricos

## Referencias

- [Documentación oficial ESP32-C6](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/)
- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/index.html)
- [Datasheet ESP32-C6](https://www.espressif.com/sites/default/files/documentation/esp32-c6_datasheet_en.pdf)
