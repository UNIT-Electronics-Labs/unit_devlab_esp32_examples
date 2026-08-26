<script setup>
import { withBase } from 'vitepress'
</script>

# Entorno MicroPython

Esta página contiene exclusivamente la configuración de MicroPython con
Thonny. Si utilizarás Arduino, consulta la guía independiente de
[configuración de Arduino IDE](./02_0_duino.md).

## Instalación de MicroPython en DualMCU

Este repositorio contiene un ejemplo básico de instalación de MicroPython en la DualMCU usando el microcontrolador ESP32. El objetivo es que puedas reutilizar partes de esta implementación en tus propios proyectos.

### Configuración del entorno

Antes de comenzar, se recomienda realizar la siguiente configuración:

#### Instalar MicroPython

Esto permite cargar el firmware en la DualMCU usando [Thonny IDE](https://thonny.org/).

1.  Ve a **"Run"** -> **"Configure interpreter"** para completar la configuración.

<div id="figure_configure_interpreter">

<figure>
<img :src="withBase('/sphinx-static/config_interpreter.png')" class="align-center"
style="width:60.0%" alt="config_interpreter.png" />
<figcaption>Configuración del intérprete</figcaption>
</figure>

</div>

### Actualización de firmware

### ESP32

Para usar MicroPython se recomienda actualizar el firmware. Sigue estos pasos para actualizar tu DualMCU ESP32:

1.  Inicia tu DualMCU ESP32 presionando el botón FLASH.

2.  Haz clic en **"Instalar o actualizar MicroPython"**.

3.  Se abrirá una ventana nueva. Usa la siguiente configuración:

    > - **Variante**: Espessif ESP32/WROOM
    > - **Versión**: 1.20.0

<div id="figure_installer">

<figure>
<img :src="withBase('/sphinx-static/installer.png')" class="align-center"
style="width:70.0%" alt="installer.png" />
<figcaption>Configuración del instalador ESP32</figcaption>
</figure>

</div>

4.  Presiona **Instalar** y espera a que termine la instalación.

Estos pasos permiten actualizar y configurar correctamente MicroPython en tu DualMCU ESP32.

### RP2040

Para usar MicroPython se recomienda actualizar el firmware. Sigue estos pasos para actualizar tu DualMCU RP2040:

1.  Inicia tu DualMCU RP2040 presionando el botón FLASH.
2.  Haz clic en **"Instalar o actualizar MicroPython"**.

3.  Se abrirá una ventana nueva. Usa la siguiente configuración:

> - **Variante**: Raspberry Pi Pico / Pico H
> - **Versión**: 1.23.0

<div id="figure_rp2040_installer">

<figure>
<img :src="withBase('/sphinx-static/rp2040_installer.png')" class="align-center"
style="width:70.0%" alt="rp2040_installer.png" />
<figcaption>Configuración del instalador RP2040</figcaption>
</figure>

</div>

4.  Presiona **Instalar** y espera a que termine la instalación.

Estos pasos permiten actualizar y configurar correctamente MicroPython en tu DualMCU ESP32.

### Ejecutar el ejemplo

Una vez configurado el entorno, abre Thonny y ejecuta el ejemplo siguiendo estos pasos:

1.  Ve a la esquina inferior derecha y selecciona la opción **"MicroPython (ESP32)"**.

<div id="figure_select_interpreter">

<figure>
<img :src="withBase('/sphinx-static/esp32_thonny.png')" class="align-center"
style="width:60.0%" alt="esp32_thonny.png" />
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
