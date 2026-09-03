# Lab 7: PULSAR Stick — I²C y Zigbee

La **PULSAR Stick** es un desarrollo pensado para facilitar la experiencia
con microcontroladores a la hora de realizar conexiones. Integra un
conector de salida rápida **Qwiic I²C** para implementar sensores, lo que
facilita la comprensión de los conceptos.

El formato "stick" es un formato de desarrollo pensado como banco de
pruebas I²C, con un botón I²C que agiliza el desarrollo. Consta de un
sistema principal **PULSAR C6**, elaborado con una conexión I²C a un
display OLED y una salida Qwiic (`SDA` en el pin 6, `SCL` en el pin 7),
con el fin de facilitar la comprensión de los conceptos. Además, se
considera **Zigbee** como una experiencia compartida para este producto.

![PULSAR Stick](./pulsar_stick/IMG_9435.jpg)

### Pantalla OLED de estado

El display OLED integrado reporta en todo momento el estado del
dispositivo, sin depender de un depurador ni del monitor serial:

- `ZIGBEE SWITCH` — rol/función activa del firmware cargado.
- `NETWORK: JOINED` — estado de la unión a la red Zigbee.
- `MODE: POWER` — modo de operación configurado.
- `SHORT: ON/OFF` — acción asignada a la pulsación corta del botón.

Esta lectura en pantalla es, junto con el patrón de LED, una de las dos
formas de obtener evidencia del identificador que se piden en los
resultados de aprendizaje.

## Objetivo del taller

Configurar y validar dos funciones embebidas principales en el
**PY32F003**, integrando su control local con una red Zigbee a través de
la interfaz I²C administrada por la **PULSAR ESP32-C6**.

## Resultados del aprendizaje

Al finalizar, los participantes podrán:

- Modificar parámetros previstos en un firmware embebido existente.
- Modificar, compilar y cargar ejemplos en dos placas PY32F003 con
  responsabilidades diferentes.
- Modificar, compilar y cargar el firmware de una PULSAR ESP32-C6
  mediante Arduino IDE.
- Asignar y comprobar los identificadores lógicos y las direcciones I²C
  previstas.
- Obtener evidencia del identificador mediante un patrón de LED definido
  o una salida de terminal, sin depender de un depurador.
- Generar un evento repetible con un temporizador de software, variar
  una lectura ADC y observar su tratamiento en los tres firmwares.
- Validar una lectura I²C local cuando la topología Qwiic completa haya
  superado la prueba previa.
- Diferenciar un identificador lógico de una dirección I²C y de una
  identidad Zigbee.
- Identificar las funciones de coordinador, enrutador y dispositivo
  final en una red Zigbee.
- Incorporar la PULSAR ESP32-C6 del equipo y observar su reporte en la
  red Zigbee.
- Distinguir una demostración de arquitectura de una integración
  eléctrica y de firmware real.

## Metodología

| Etapa | Asignación de roles | Dinámica |
|---|---|---|
| **Etapa 1: Preparación** | 2 personas en PY32 A · 2 personas en PY32 B · 1 persona en ESP32-C6 | En cada pareja se alternan para modificar, compilar y cargar el firmware.<br>**PY32:** flujo separado (compilación en Arduino IDE + carga SWD vía Web/Chrome).<br>**ESP32-C6:** flujo directo en Arduino IDE. |
| **Etapa 2: Integración** | Roles rotativos entre los 5 integrantes | El equipo completo integra la estación, realiza pruebas globales y aplica el procedimiento de recuperación si es necesario. |

## Materiales para el taller

| Producto | SKU | Cantidad por equipo | Cantidad total |
|---|---|---|---|
| Tarjeta PY32F003L24D6TR ARM 32 bits | AR4354 | 1 | 4 |
| PULSAR ESP32-C6 | AR4606 | 1 | 5 |
| CH552 programador multiprotocolo SWD | AR4664 | 2 | 8 |
| Protoboard de 400 puntos, variante blanca | N/D — producto con variantes | 1 | 4 |
| Potenciómetro lineal WH148 de 3 pines (10k) | N/D — seleccionar 10k | 1 | 4 |
| Arnés Qwiic de 4 pines, pitch 1 mm | N/D — producto con variantes | 3 | 12 |
| UNIT Hub I2C QW/ST de 5 puertos | AR3678 | 1 | 4 |

## Carga de firmware — Panel Loader

Los dispositivos de la práctica se programan desde el navegador con
**Panel Loader**, sin instalar `esptool` ni drivers adicionales. Elige
el firmware y el protocolo que correspondan al microcontrolador y a la
función de cada placa:

**[Abrir Panel Loader](https://unit-electronics-labs.github.io/unit_microsupport_labs/)**

> Requiere un navegador con soporte WebSerial/WebUSB (Chrome o Edge de
> escritorio). Firefox y Safari no son compatibles.

### Descarga de firmware merged

Estos binarios ya contienen las imágenes necesarias en un solo archivo
y están listos para seleccionarse desde Panel Loader:

![Aplicación de los firmware merged para PULSAR Stick](./pulsar_stick/firmware-merged-pulsar-stick-v2.png)

| Firmware | Aplicación | Archivo de descarga |
|---|---|---|
| Gateway | Para ESP32-C5 | [Descargar `pulsar_stick_gateway_merged.bin`](/examples/firmware/pulsar_stick_gateway_merged.bin) |
| Switch | Para una sola Stick con ESP32-C6 | [Descargar `pulsar_stick_switch_merged.bin`](/examples/firmware/pulsar_stick_switch_merged.bin) |
| Lamp | Para todas las PULSAR Stick | [Descargar `pulsar_stick_lamp_merged.bin`](/examples/firmware/pulsar_stick_lamp_merged.bin) |
| Toolbox | Herramienta de pruebas para validación de hardware | [Descargar `pulsar_stick_TOOLBOX_merged.bin`](/examples/firmware/pulsar_stick_TOOLBOX_merged.bin) |

### Protocolo según la placa

| Pestaña del panel | Protocolo | Placa de esta práctica |
|---|---|---|
| **Cortex** | SWD, vía el programador **CH552** | Placas **PY32F003**, cuando la etapa de la práctica requiera programarlas |
| **ESP32** | Serie / esptool-js | **Gateway ESP32-C5** y PULSAR Stick **ESP32-C6** con firmware Switch, Lamp o Toolbox |

### Pasos para cargar un firmware

1. Abre el enlace del Panel Loader y selecciona la pestaña **Cortex** o
   **ESP32** según la placa que vayas a programar.
2. En **Firmware**, da clic en **Seleccionar firmware** y elige el
   `.bin` (o `.elf` en Cortex) que compilaste en Arduino IDE:
   - **Archivo local**: el binario recién compilado en tu equipo.
   - **Catálogo web**: una versión ya publicada, si el instructor la
     agregó al catálogo del repositorio.
3. Revisa la **dirección de flash** (`0x0000` por defecto) y la
   **velocidad de carga** (115200 baud es un valor seguro para
   empezar).
4. Da clic en **Conectar ESP** (o el botón equivalente en Cortex) y
   selecciona el puerto o dispositivo que te ofrezca el navegador.
5. Da clic en **Programar firmware** y espera a que el progreso llegue
   a 100%.
6. Repite el proceso con las demás placas según la función asignada a
   cada una.

### Programación por estaciones (varios equipos en paralelo)

En modo **ESP32** el panel ofrece una grilla de hasta **10 estaciones**
para conectar y programar varias PULSAR ESP32-C6 a la vez desde una
sola computadora — útil en la Etapa 1 del taller, cuando varias parejas
programan su placa al mismo tiempo:

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

## Vista del producto

La carcasa de la PULSAR Stick está impresa en 3D y disponible en dos
acabados de color; el hardware y las conexiones son idénticos en ambos
casos.

| | Variante roja | Variante beige |
|---|---|---|
| **Frente** — display OLED y botón BOOT | ![PULSAR Stick roja, frente](./pulsar_stick/IMG_9435.jpg) | ![PULSAR Stick beige, frente](./pulsar_stick/IMG_9437.jpg) |
| **Reverso** — grabado de marca | ![PULSAR Stick roja, reverso](./pulsar_stick/IMG_9436.jpg) | ![PULSAR Stick beige, reverso](./pulsar_stick/IMG_9439.jpg) |
| **Con módulo LED Qwiic** conectado | ![PULSAR Stick roja con LED Qwiic](./pulsar_stick/IMG_9440.jpg) | ![PULSAR Stick beige con LED Qwiic](./pulsar_stick/IMG_9441.jpg) |

La salida Qwiic (`SDA` pin 6, `SCL` pin 7) permite conectar módulos como
el anillo de LEDs mostrado arriba para validar rápidamente el bus I²C
antes de integrar los sensores propios del taller.
