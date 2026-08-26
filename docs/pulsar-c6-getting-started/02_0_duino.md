<script setup>
import { withBase } from 'vitepress'
</script>

# Arduino IDE para Pulsar C6

Arduino IDE es el flujo de trabajo recomendado para las prácticas y ejemplos de la placa PULSAR C6 (ESP32-C6). Esta página explica cómo instalar el IDE y agregar los paquetes de placas de UNIT Electronics necesarios para reconocer la PULSAR C6.

## 1. Instalar Arduino IDE

Descarga e instala Arduino IDE 1.8 o superior (se recomienda la versión 2.x) desde el <a href="https://www.arduino.cc/en/software" target="_blank">sitio web oficial de Arduino</a>.

## 2. Agregar las URLs del gestor de placas de UNIT Electronics

Arduino permite instalar paquetes de plataformas de terceros mediante el **Gestor de placas**. UNIT Electronics distribuye un índice JSON por cada familia de microcontroladores que soporta.

1.  Abre Arduino IDE y ve a **Archivo > Preferencias**.

<figure>
<img :src="withBase('/sphinx-static/menu-preferencias.png')" class="align-center"
style="width:80.0%" alt="menu-preferencias.png" />
<figcaption>Archivo &gt; Preferencias</figcaption>
</figure>

2.  En el campo **Gestor de URLs Adicionales de Tarjetas**, pega la URL del paquete que necesites. Puedes agregar varias URLs separadas por comas.

<div class="note">

<div class="title">

Nota

</div>

Para la placa **PULSAR C6** (ESP32-C6) usa el paquete **ESP32**. Las demás URLs se listan como referencia para otras placas de UNIT Electronics.

</div>

| Placa / familia | URL del índice de paquetes |
|---|---|
| **ESP32** (usar para PULSAR C6) | `https://raw.githubusercontent.com/UNIT-Electronics/Uelectronics-ESP32-Arduino-Package/main/package_Uelectronics_esp32_index.json` |
| RP2040 | `https://raw.githubusercontent.com/UNIT-Electronics/Uelectronics-RP2040-Arduino-Package/main/package_Uelectronics_rp2040_index.json` |
| CH552 / MCS51 | `https://raw.githubusercontent.com/UNIT-Electronics/Uelectronics-CH552-Arduino-Package/refs/heads/main/package_duino_mcs51_index.json` |
| PY32 | `https://raw.githubusercontent.com/UNIT-Electronics-MX/unit_electronics_py32_arduino_package/main/package_unit_electronics_py32_index.json` |
| ATtiny | `https://raw.githubusercontent.com/UNIT-Electronics-MX/uelectronics_attinycore_arduino_package/refs/heads/main/package_uelectronics_attiny_custom.json` |

<figure>
<img :src="withBase('/sphinx-static/menu-preferencias-2.png')" class="align-center"
style="width:80.0%" alt="menu-preferencias-2.png" />
<figcaption>Varias URLs en el Gestor de URLs Adicionales de Tarjetas</figcaption>
</figure>

3.  Presiona **OK** para cerrar el cuadro de diálogo.

## 3. Instalar el paquete ESP32 desde el Gestor de placas

1.  Ve a **Herramientas > Placa > Gestor de placas**.
2.  Escribe "esp32" en el buscador y selecciona el paquete de UNIT Electronics / Espressif Systems para instalarlo.

<figure>
<img :src="withBase('/sphinx-static/BoardsManager.png')" class="align-center"
style="width:60.0%" alt="BoardsManager.png" />
<figcaption>Gestor de placas</figcaption>
</figure>

3.  Una vez instalado, ve a **Herramientas > Placa** y selecciona **ESP32C6 Dev Module** (o la entrada correspondiente a PULSAR C6, si el paquete la incluye).

<div class="attention">

<div class="title">

Atención

</div>

Verifica que el puerto serie correcto esté seleccionado en **Herramientas > Puerto** antes de compilar y cargar un sketch.

</div>

## Paquetes de UNIT Electronics para Arduino

| Paquete | Repositorio |
|---|---|
| ESP32 | <https://github.com/UNIT-Electronics/Uelectronics-ESP32-Arduino-Package> |
| RP2040 | <https://github.com/UNIT-Electronics/Uelectronics-RP2040-Arduino-Package> |
| CH552 / MCS51 | <https://github.com/UNIT-Electronics/Uelectronics-CH552-Arduino-Package> |
| PY32 | <https://github.com/UNIT-Electronics-MX/unit_electronics_py32_arduino_package> |
| ATtiny | <https://github.com/UNIT-Electronics-MX/uelectronics_attinycore_arduino_package> |

## Documentación adicional

Consulta la <a href="https://wiki.uelectronics.com/tutoriales/inicio-arduino" target="_blank">guía de instalación de paquetes de UNIT Electronics</a> para más detalle y capturas de pantalla por sistema operativo.
