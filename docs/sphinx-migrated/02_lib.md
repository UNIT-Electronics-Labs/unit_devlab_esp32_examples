# Instalación de paquetes - MicroPython

Esta sección te guía durante la instalación de las librerías requeridas usando el gestor de paquetes [pip](https://pip.pypa.io/en/stable/).

## Guía de instalación usando la librería MIP

<div class="note">

<div class="title">

Nota

</div>

La librería `mip` se utiliza para instalar otras librerías en la placa **PULSAR**.

</div>

### Requisitos

- Dispositivo ESP32C6
- Thonny IDE
- Credenciales Wi-Fi (SSID y contraseña)

### Instrucciones de instalación

Sigue estos pasos para instalar las librerías `max1704x.py`, `ssd1306.py` y `sdcard.py`:

### Conectar a Wi-Fi

Copia y ejecuta el siguiente código en Thonny para conectar tu ESP32 a una red Wi-Fi:

``` python
import mip
import network
import time

def connect_wifi(ssid, password):
   wlan = network.WLAN(network.STA_IF)
   wlan.active(True)
   wlan.connect(ssid, password)

   for _ in range(10):
      if wlan.isconnected():
            print('Connected to the Wi-Fi network')
            return wlan.ifconfig()[0]
      time.sleep(1)

   print('Could not connect to the Wi-Fi network')
   return None

ssid = "your_ssid"
password = "your_password"

ip_address = connect_wifi(ssid, password)
print(ip_address)
mip.install('https://raw.githubusercontent.com/UNIT-Electronics/MAX1704X_lib/refs/heads/main/Software/MicroPython/example/max1704x.py')
mip.install('https://raw.githubusercontent.com/UNIT-Electronics-MX/unit_devlab_micropython_libraries/refs/heads/main/software/ssd1306/ssd1306.py')
mip.install('https://raw.githubusercontent.com/UNIT-Electronics-MX/unit_devlab_micropython_libraries/refs/heads/main/software/sdcard/sdcard.py')
```

<figure>
<img src="/sphinx-static/libraries.png" class="align-center"
style="width:100.0%" alt="/sphinx-static/libraries.png" />
<figcaption>Instalación de librerías con MIP</figcaption>
</figure>
