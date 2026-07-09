# Configuración del entorno

## Instalación de MicroPython en DualMCU

Este repositorio contiene un ejemplo básico de instalación de MicroPython en la DualMCU usando el microcontrolador ESP32. El objetivo es que puedas reutilizar partes de esta implementación en tus propios proyectos.

### Configuración del entorno

Antes de comenzar, se recomienda realizar la siguiente configuración:

#### Instalar MicroPython

Esto permite cargar el firmware en la DualMCU usando [Thonny IDE](https://thonny.org/).

1.  Ve a **"Run"** -> **"Configure interpreter"** para completar la configuración.

<div id="figure_configure_interpreter">

<figure>
<img src="/sphinx-static/config_interpreter.png" class="align-center"
style="width:60.0%" alt="/sphinx-static/config_interpreter.png" />
<figcaption>Configuración del intérprete</figcaption>
</figure>

</div>

### Actualización de firmware

### ESP32

Para usar MicroPython se recomienda actualizar el firmware. Sigue estos pasos para actualizar tu DualMCU ESP32:

1.  Inicia tu DualMCU ESP32 presionando el botón FLASH.

2.  Haz clic en **"Install or Update MicroPython"**.

3.  Se abrirá una ventana nueva. Usa la siguiente configuración:

    > - **Variante**: Espessif ESP32/WROOM
    > - **Versión**: 1.20.0

<div id="figure_installer">

<figure>
<img src="/sphinx-static/installer.png" class="align-center"
style="width:70.0%" alt="/sphinx-static/installer.png" />
<figcaption>Configuración del instalador ESP32</figcaption>
</figure>

</div>

4.  Presiona **Install** y espera a que termine la instalación.

Estos pasos permiten actualizar y configurar correctamente MicroPython en tu DualMCU ESP32.

### RP2040

Para usar MicroPython se recomienda actualizar el firmware. Sigue estos pasos para actualizar tu DualMCU RP2040:

1.  Inicia tu DualMCU RP2040 presionando el botón FLASH.
2.  Haz clic en **"Install or Update MicroPython"**.

3.  Se abrirá una ventana nueva. Usa la siguiente configuración:

> - **Variante**: Raspberry Pi Pico / Pico H
> - **Versión**: 1.23.0

<div id="figure_rp2040_installer">

<figure>
<img src="/sphinx-static/rp2040_installer.png" class="align-center"
style="width:70.0%" alt="/sphinx-static/rp2040_installer.png" />
<figcaption>Configuración del instalador RP2040</figcaption>
</figure>

</div>

4.  Presiona **Install** y espera a que termine la instalación.

Estos pasos permiten actualizar y configurar correctamente MicroPython en tu DualMCU ESP32.

### Ejecutar el ejemplo

Una vez configurado el entorno, abre Thonny y ejecuta el ejemplo siguiendo estos pasos:

1.  Ve a la esquina inferior derecha y selecciona la opción **"MicroPython (ESP32)"**.

<div id="figure_select_interpreter">

<figure>
<img src="/sphinx-static/esp32_thonny.png" class="align-center"
style="width:60.0%" alt="/sphinx-static/esp32_thonny.png" />
<figcaption>Intérprete ESP32</figcaption>
</figure>

</div>

#### Ejemplo Blink

Dentro de la carpeta **Examples** encontrarás un ejemplo básico llamado "blink" para verificar que la configuración se aplicó correctamente.

``` python
'''
Unit Electronics 2023
       (o_
(o_    //\
(/)_   V_/_ 

version: 0.0.1
revision: 0.0.1
context: This code is a basic configuration of three RGB LEDs
'''
import machine
import time

led_pin = machine.Pin(4, machine.Pin.OUT)
led_pin2 = machine.Pin(26, machine.Pin.OUT)
led_pin3 = machine.Pin(25, machine.Pin.OUT)

def loop():
     while True:
        led_pin.on()    
        led_pin2.on()   
        led_pin3.on()  
        time.sleep(1)  
        led_pin.off()   
        led_pin2.off()  
        led_pin3.off()  
        time.sleep(1)   

loop()
```

## Entorno de instalación de Arduino IDE

### Uelectronics-RP2040-Arduino-Package

El core Arduino de Uelectronics es una versión portada de [Raspberry Pi Pico Arduino Core](https://github.com/earlephilhower/arduino-pico), basada en el trabajo de Earle F. Philhower III. Este port para RP2040 usa el SDK de Raspberry Pi Pico y una toolchain GCC 10.3/Newlib 4.0 personalizada, igual que la [versión 2.6.4](https://github.com/earlephilhower/arduino-pico/releases/tag/2.6.4) de earlephilhower.

## Documentación

Consulta <https://github.com/UNIT-Electronics/DualMCU> y sus ejemplos para información de uso más detallada.

## Placas soportadas

- DualMCU RP2040
- Raspberry Pi Pico
- Raspberry Pi Pico W
- Generic (flash y pines de E/S configurables)

## Instalación desde Arduino Boards Manager

Abre Arduino IDE y ve a File -> Preferences.

En el cuadro de diálogo, ingresa la siguiente URL en el campo "Additional Boards Manager URLs":

<https://github.com/UNIT-Electronics/Uelectronics-RP2040-Arduino-Package/releases/download/v1.0.0/package_Uelectronics_rp2040_index.json>

<div id="figure_AditionalBoardsManagerURL">

<figure>
<img src="/sphinx-static/AditionalBoardsManagerURL.png"
class="align-center" style="width:60.0%"
alt="/sphinx-static/AditionalBoardsManagerURL.png" />
<figcaption>Preferencias: Additional Boards Manager URL</figcaption>
</figure>

</div>

Presiona OK para cerrar el cuadro de diálogo.

Ve a Tools -> Boards -> Board Manager en el IDE.

Escribe "DualMCU" en el cuadro de búsqueda y selecciona "Add":

<div id="figure_BoardsManager">

<figure>
<img src="/sphinx-static/BoardsManager.png" class="align-center"
style="width:60.0%" alt="/sphinx-static/BoardsManager.png" />
<figcaption>BoardsManager</figcaption>
</figure>

</div>

### Uelectronics-RP2040-Arduino-Package

El paquete Uelectronics-ESP32-Arduino reúne herramientas de software para programar y controlar dispositivos con el MCU ESP32 en la DualMCU desde la plataforma Arduino. Incluye librerías y herramientas para programar el ESP32 usando Arduino IDE.

El paquete incluye ejemplos de código para comenzar a programar el ESP32 y crear dispositivos conectados.

## Placas soportadas

- UNIT DualMCU ESP32
- ESP32 Dev Module
- ESP32S3 Dev Module
- ESP32C3 Dev Module
- ESP32S2 Dev Module

## Instalación desde Arduino Boards Manager

Abre Arduino IDE y ve a File -> Preferences.

- Enlace de versión estable:
  `https://github.com/UNIT-Electronics/Uelectronics-ESP32-Arduino-Package/releases/download/v1.0.0/package_Uelectronics_ESP32_index.json`

Arduino permite instalar paquetes de plataformas de terceros mediante Boards Manager.

- Instala Arduino IDE 1.8 o superior. La versión actual está disponible en el [sitio web de Arduino](http://www.arduino.cc/en/main/software).
- Inicia Arduino y abre la ventana de preferencias.

- Ingresa uno de los enlaces anteriores en el campo *Additional Board Manager URLs*. Puedes agregar varias URLs separadas por comas.
- Abre Boards Manager desde Tools > Board e instala la plataforma *esp32*. Después de instalarla, selecciona tu placa ESP32 desde Tools > Board.

### Soporte

La placa de desarrollo DualMCU es compatible con entornos MicroPython como Thonny y con el entorno de desarrollo Arduino. Esta compatibilidad permite programar la DualMCU usando MicroPython, CircuitPython o el lenguaje Arduino.

El soporte de IDE para MicroPython incluye una consola interactiva (REPL) para ejecutar comandos de inmediato en MicroPython y CircuitPython, facilitando pruebas y depuración rápidas.

Además, el soporte para Arduino permite aprovechar sus herramientas y recursos de comunidad. Es especialmente útil si ya estás familiarizado con Arduino y quieres usar sus funciones y recursos para desarrollar proyectos con la placa DualMCU.

|                            |                                                                             |
|----------------------------|-----------------------------------------------------------------------------|
| **Arduino Package RP2040** | **https://github.com/UNIT-Electronics/Uelectronics-RP2040-Arduino-Package** |
| **Arduino Package ESP32**  | **https://github.com/UNIT-Electronics/Uelectronics-ESP32-Arduino-Package**  |
