# Lab 8: PULSAR Powersupply — Zigbee + Integración con Alexa

**PULSAR Powersupply** convierte una placa **Pulsar C6** en una fuente
de voltaje controlada por voz. Sobre la salida Qwiic I²C se conecta un
módulo **HUSB238** — un disparador (*trigger*) de USB-C Power
Delivery — capaz de negociar distintos voltajes (5 V, 9 V, 12 V, 15 V,
18 V o 20 V) frente a un cargador USB-C compatible con PD.

El firmware ya viene compilado en un binario listo para cargar con
**Panel Loader**, así que no se necesita instalar ningún entorno de
desarrollo ni tocar una sola línea de código para completar esta
práctica.

La integración se diseñó y probó con **Alexa** (un Echo con hub Zigbee
integrado, como el Echo Dot Max): al unirse, el dispositivo aparece en
la app de Alexa como una luz más — `Alexa, enciende la fuente` activa
la salida en el último voltaje solicitado, y `Alexa, pon la fuente al
50%` reconfigura el voltaje según el nivel pedido, tal como si se
tratara del brillo de un foco regulable.

## Objetivo del taller

Cargar el firmware `power_supply_zigbee` en una Pulsar C6 con un módulo
HUSB238 conectado, emparejarla con Alexa y comprobar que el voltaje de
salida se puede controlar por voz o desde la app.

## Resultados del aprendizaje

Al finalizar, los participantes podrán:

- Explicar para qué sirve un módulo HUSB238 y qué es un contrato de
  Power Delivery (PD).
- Conectar correctamente el HUSB238 a la Pulsar C6 mediante Qwiic.
- Cargar el firmware `power_supply_zigbee` con Panel Loader.
- Emparejar el dispositivo con Alexa y reconocerlo en la app como una
  luz regulable.
- Controlar el voltaje de salida con comandos de voz o desde el control
  de brillo de la app de Alexa.
- Entender por qué el porcentaje de brillo no representa "más o menos
  energía", sino la selección de uno de los voltajes disponibles.
- Resolver los problemas más comunes de emparejamiento.

## Materiales para el taller

| Producto | Cantidad | Función |
|---|---|---|
| PULSAR C6 (ESP32-C6) | 1 | Ejecuta el firmware `power_supply_zigbee` |
| Módulo HUSB238 (trigger USB-C PD) | 1 | Negocia el contrato de voltaje con el cargador |
| Cargador USB-C con Power Delivery | 1 | Debe anunciar al menos un perfil de los soportados (5/9/12/15/18/20 V) |
| Cable Qwiic (JST-SH 4 pines) | 1 | Conecta la Pulsar C6 con el HUSB238 |
| Echo con hub Zigbee integrado (Echo Dot Max u otro modelo con hub Zigbee) | 1 por red | Forma la red Zigbee y expone el dispositivo a Alexa |
| Cable USB de datos | 1 | Programación mediante Panel Loader |

## Conexión del HUSB238

El HUSB238 se conecta directamente a la salida Qwiic de la Pulsar C6
con un cable Qwiic (4 pines: `SDA`, `SCL`, 3V3 y GND) — no requiere
cableado manual ni soldadura. Conecta también el cargador USB-C con PD
al puerto de entrada del HUSB238 **antes de encender la placa**, para
que el firmware detecte al arrancar qué voltajes admite ese cargador.

## Carga de firmware — Panel Loader

Al igual que en el Lab 7, la Pulsar C6 se programa desde el navegador con
**Panel Loader**, sin instalar `esptool` ni drivers adicionales.

**[Abrir Panel Loader](https://unit-electronics-labs.github.io/unit_microsupport_labs/)**

> Requiere un navegador con soporte WebSerial/WebUSB (Chrome o Edge de
> escritorio). Firefox y Safari no son compatibles.

### Descarga de firmware merged

Este binario ya contiene las imágenes necesarias (bootloader, tabla de
particiones y aplicación) en un solo archivo, listo para seleccionarse
desde Panel Loader:

| Firmware | Aplicación | Archivo de descarga |
|---|---|---|
| Power Supply | Para la Pulsar C6 con módulo HUSB238 por Qwiic | [Descargar `power_supply_zigbee_merged.bin`](/examples/firmware/power_supply_zigbee_merged.bin) |

### Protocolo de carga

| Pestaña del panel | Protocolo | Placa de esta práctica |
|---|---|---|
| **ESP32** | Serie / esptool-js | Pulsar C6 (ESP32-C6) con firmware Power Supply |

### Pasos para cargar el firmware

1. Abre el enlace del Panel Loader y selecciona la pestaña **ESP32**.
2. En **Firmware**, da clic en **Seleccionar firmware** y elige el
   archivo `power_supply_zigbee_merged.bin` descargado.
3. Revisa la **dirección de flash** (`0x0000` por defecto) y la
   **velocidad de carga** (115200 baud es un valor seguro para
   empezar).
4. Da clic en **Conectar ESP** y selecciona el puerto que te ofrezca el
   navegador.
5. Da clic en **Programar firmware** y espera a que el progreso llegue
   a 100%.
6. Conecta el módulo HUSB238 por Qwiic y un cargador PD antes de
   encender la placa, para que detecte los voltajes disponibles al
   arrancar.

### Programación por estaciones (varios equipos en paralelo)

En modo **ESP32** el panel ofrece una grilla de hasta **10 estaciones**
para conectar y programar varias Pulsar C6 a la vez desde una sola
computadora — útil si varias parejas programan su placa al mismo
tiempo:

1. Da clic en **Conectar ESP** dentro de cada estación para asignarle
   un dispositivo.
2. Usa **Seleccionar conectados** para marcar todas las estaciones con
   placa detectada.
3. Da clic en **Programar seleccionados** para cargar el mismo
   firmware en todas las estaciones marcadas en un solo paso.

El resumen inferior (**OK · SIN FLASH · ERROR**) indica cuántas
estaciones terminaron correctamente, y la consola **esptool / ESP** en
la parte inferior del panel muestra el log en tiempo real — revísala si
una carga falla o un dispositivo no queda en **OK**.

## Emparejar con Alexa

1. Conecta el HUSB238 por Qwiic y un cargador PD antes de encender la
   placa, para que detecte los voltajes reales que ese cargador admite.
2. En la app de Alexa, inicia la búsqueda de dispositivos
   (**Dispositivos → Agregar dispositivo → Zigbee**, en un Echo con hub
   Zigbee integrado) **antes** de energizar o reiniciar la Pulsar C6.
3. Espera unos segundos a que la placa se una a la red y Alexa la
   detecte.
4. Confirma en la app que el dispositivo aparece como una luz regulable
   (fabricante `DevLab`, modelo `POWER_SUPPLY_PD`) y termina el
   registro si Alexa lo pide (por ejemplo, asignarle un nombre o una
   habitación).
5. Prueba por voz:
   - `Alexa, enciende [nombre del dispositivo]` → reactiva la salida en
     el último voltaje conocido.
   - `Alexa, pon [nombre del dispositivo] al 100%` → selecciona el mayor
     voltaje que el cargador conectado admite.
   - `Alexa, pon [nombre del dispositivo] al 50%` → selecciona un
     voltaje intermedio entre los que el cargador anuncia.
   - `Alexa, apaga [nombre del dispositivo]` → marca la salida como
     apagada, sin desconectar físicamente el cargador.

## Usar el control de brillo (dimmer) en la app de Alexa

Además de los comandos de voz, el voltaje se puede ajustar tocando el
control deslizante de brillo dentro de la app, igual que en cualquier
foco regulable:

1. Abre la app de Alexa y entra al dispositivo
   (`Dispositivos → [nombre del dispositivo]`).
2. Debajo del botón de encendido/apagado aparece un control deslizante
   de brillo — arrástralo para elegir el voltaje, o toca un porcentaje
   específico si la app lo permite.
3. Suelta el control: la app envía el nuevo nivel y el dispositivo
   selecciona el voltaje más cercano disponible (ver
   [Prueba de funcionamiento](#prueba-de-funcionamiento)).

![Control de brillo del Power Supply en la app de Alexa](./pulsar_powersupply/dimmer-alexa.png)

> Si no ves el control deslizante, revisa que el dispositivo se haya
> emparejado como **luz regulable** y no como un simple enchufe/switch;
> vuelve a intentar el emparejamiento si Alexa lo registró como On/Off
> simple.

## Prueba de funcionamiento

- El porcentaje de brillo que muestra Alexa **no** es intensidad de luz:
  cada tramo corresponde a uno de los voltajes que el cargador conectado
  anuncia (por ejemplo, con un cargador que sólo admite 5 V y 9 V, 0–49%
  selecciona 5 V y 50–100% selecciona 9 V).
- Si pides un porcentaje que no corresponde exactamente a un voltaje
  disponible, el dispositivo selecciona el más cercano que el cargador sí
  admite.
- `OFF` no desconecta el cargador ni corta el voltaje que ya se negoció:
  sólo marca la salida como apagada en Alexa. Para cambiar de voltaje de
  nuevo basta con pedir un nuevo porcentaje o volver a encenderlo.
- Si cambias de cargador, reinicia la placa para que vuelva a detectar
  los voltajes que el nuevo cargador admite.

## Si el dispositivo no aparece en Alexa

1. Verifica que el cargador PD esté conectado al HUSB238 **antes** de
   encender la Pulsar C6.
2. Cierra y vuelve a abrir la búsqueda de dispositivos Zigbee en la app
   de Alexa, y acerca la placa al Echo durante el emparejamiento.
3. Si la placa ya se emparejó antes con otra red o dejó de responder,
   vuelve a cargar el firmware desde Panel Loader (borra el
   emparejamiento anterior y la placa vuelve a intentar unirse desde
   cero).
4. Elimina el dispositivo desde la app de Alexa si quedó registrado
   como "no disponible" y repite el proceso de emparejamiento.
