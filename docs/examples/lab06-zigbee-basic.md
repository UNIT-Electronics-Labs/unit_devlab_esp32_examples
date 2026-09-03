# Lab 6: Red Zigbee entre dos Pulsar C6

En esta práctica aprenderás a usar Zigbee para comunicarte con dispositivos y crear aplicaciones IoT de bajo consumo. 

1.0.0

## Objetivos

- Configurar correctamente una **Pulsar C6** como **Coordinador**
- Configurar correctamente una **Pulsar C6** como **End Device**
- Crear una **red de malla simple** Zigbee
- Mandar datos desde el End Device hacia el Coordinador
- Leer e imprimir los datos desde el Coordinador en el monitor serial

## Materiales

- Arduino IDE
- Dos placas Pulsar C6
- Dos cables USB tipo C

## 1. Introducción

Zigbee es un protocolo de comunicación inalámbrica de bajo consumo,
pensado para redes de sensores y domótica (por ejemplo, focos, sensores
de temperatura, cerraduras inteligentes, etc.). A diferencia del Wi-Fi,
los dispositivos Zigbee forman una **red de malla** entre sí y consumen
muy poca energía, lo que los hace ideales para sensores que funcionan con
batería durante meses o años.

El microcontrolador **ESP32-C6** de Espressif trae un
radio 802.15.4 integrado, que es el mismo tipo de radio que usa Zigbee.
Gracias a eso, se pueden programar con Arduino IDE para actuar como
dispositivos Zigbee reales.

En esta práctica vas a construir un ejemplo mínimo pero completo:

- Una placa Pulsar C6 que **cuenta** (incrementa un número cada segundo) y
  lo envía por la red Zigbee.
- Otra placa Pulsar C6 que **recibe** ese número y lo muestra por el
  puerto Serial.


## 2. Qué vas a lograr al final

Al terminar esta práctica tendrás:

1. Un **end device** Zigbee (la placa que "cuenta") funcionando y
   enviando su valor por la red.
2. Un **coordinador** Zigbee (la placa que "escucha") recibiendo ese
   valor y mostrándolo por Serial en tiempo real.
3. Un entendimiento básico de los conceptos de Zigbee necesarios para
   extender este ejemplo a sensores reales más adelante.

## 3. Conceptos previos

Antes del código, conviene tener claros algunos conceptos de
Zigbee. Si ya los conoces, puedes saltar a la sección 4.

### 3.1 Roles de los dispositivos en una red Zigbee

Una red Zigbee siempre tiene tres tipos de rol posibles:

- **Coordinador (Coordinator):** forma la red, le asigna direcciones a
  los demás dispositivos y actúa como su "centro". **Solo puede haber
  uno por red.**
- **Router:** repite mensajes para extender el alcance de la red y puede
  aceptar que otros dispositivos se unan a través de él.
- **End Device:** es el nodo más simple; se conecta a un coordinador o
  router, pero no repite mensajes de otros. Está pensado para
  dispositivos de bajo consumo (sensores a batería).

En esta práctica usamos únicamente un **coordinador** y un **end
device**.

### 3.2 Endpoints

Dentro de un mismo dispositivo Zigbee puede haber varios "endpoints"
(numerados del 1 al 254), cada uno representando una función distinta
(por ejemplo, un dispositivo podría tener un endpoint para un sensor de
temperatura y otro para un interruptor). En nuestros ejemplos cada placa
usa un único endpoint.

### 3.3 Clústeres y la ZCL (Zigbee Cluster Library)

Zigbee no envía datos "en crudo": todo dato viaja empaquetado dentro de
un **clúster**, que es una definición estandarizada de qué atributos
tiene un tipo de dispositivo y qué significan. La ZCL (Zigbee Cluster
Library) define clústeres para cosas comunes: temperatura, humedad,
interruptores on/off, nivel de batería, etc.

> Punto importante para esta práctica: **Zigbee no tiene un clúster
genérico de "contador"**. Cualquier valor que se quiera enviar tiene que
viajar dentro de alguno de los clústeres ya definidos.

### 3.4 Bind (enlace)

Para que un coordinador reciba datos de un end device, ambos deben
quedar **enlazados (bound)**: es un proceso de configuración en el que
el coordinador le dice al end device *repórtame tus datos a mí*. Este
proceso normalmente lo maneja la librería de forma automática, como
se podrá ver más adelante.

### 3.5 Reporting (reporte de atributos)

Una vez enlazados, el end device no manda su valor constantemente sin
parar: se configura una política de **reporte**, con tres parámetros
típicos:

- **Intervalo mínimo:** no reportar más seguido que esto, aunque el valor
  cambie muy rápido.
- **Intervalo máximo:** reportar al menos con esta frecuencia, aunque el
  valor no haya cambiado.
- **Delta (cambio mínimo):** reportar en cuanto el valor cambie al menos
  esta cantidad.

## 4. Reutilización de código

Como vimos en la sección 3.3, no existe un clúster de "contador". Para esta
práctica se implementó reutilizando el clúster de **Temperature
Measurement** (medición de temperatura) como transporte: el end device
manda el contador como si fuera una lectura de temperatura, y el
coordinador lo recibe y lo muestra como lo que realmente es, un
contador.

¿Por qué ese clúster y no otro? Porque el par
`ZigbeeTempSensor` (end device) / `ZigbeeThermostat` (coordinador) es,
por ahora, el más maduro y mejor soportado de la librería Zigbee de
Arduino-ESP32: ya trae resuelto el bind automático, el reporting y los
callbacks para recibir datos, sin tener que programar nada de eso a
mano.

## 5. Parte 1 — El End Device (la placa que cuenta)

### 5.1 Configuración en Arduino IDE

En el menú `Tools`, para esta placa:

| Opción | Valor |
|---|---|
| Board | Tu placa ESP32-C6 |
| Zigbee mode | `Zigbee ED (end device)` |
| Partition Scheme | `Zigbee 4MB with spiffs` (o el esquema Zigbee equivalente a tu flash) |
| Port | El puerto COM/tty de esta placa |

### 5.2 Código completo

```cpp
// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "Zigbee.h"

/* Configuración del endpoint Zigbee */
#define COUNTER_ENDPOINT_NUMBER 10
uint8_t button = BOOT_PIN;

// Reutilizamos el clúster de Temperature Sensor como transporte del contador.
ZigbeeTempSensor zbCounter = ZigbeeTempSensor(COUNTER_ENDPOINT_NUMBER);

static uint32_t counterValue = 0;

/************************ Contador *****************************/
static void counter_value_update(void *arg) {
  for (;;) {
    counterValue++;
    Serial.printf("Updated counter value to %lu\r\n", (unsigned long)counterValue);
    zbCounter.setTemperature((float)counterValue);
    delay(1000);
  }
}

/********************* Arduino functions **************************/
void setup() {
  Serial.begin(115200);
  pinMode(button, INPUT_PULLUP);

  zbCounter.setManufacturerAndModel("Espressif", "ZigbeeCounter");

  // El rango debe cubrir los valores que va a tomar el contador
  zbCounter.setMinMaxValue(0, 10000);
  zbCounter.setTolerance(1);

  Zigbee.addEndpoint(&zbCounter);

  Serial.println("Starting Zigbee...");
  if (!Zigbee.begin()) {
    Serial.println("Zigbee failed to start!");
    Serial.println("Rebooting...");
    ESP.restart();
  } else {
    Serial.println("Zigbee started successfully!");
  }

  Serial.println("Connecting to network");
  while (!Zigbee.connected()) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  xTaskCreate(counter_value_update, "counter_update", 2048, NULL, 10, NULL);

  // Reporta cada vez que el valor cambie en al menos 1 (o sea, cada segundo)
  zbCounter.setReporting(1, 0, 1);
}

void loop() {
  if (digitalRead(button) == LOW) {
    delay(100);
    int startTime = millis();
    while (digitalRead(button) == LOW) {
      delay(50);
      if ((millis() - startTime) > 3000) {
        Serial.println("Resetting Zigbee to factory and rebooting in 1s.");
        delay(1000);
        Zigbee.factoryReset();
      }
    }
    zbCounter.reportTemperature();
  }
  delay(100);
}
```

### 5.3 Explicación

**Verificación del modo Zigbee**

```cpp
#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif
```

Impide compilar si no seleccionaste `Zigbee ED` en el menú `Tools`. Es
una protección para no confundir esta placa con la del coordinador.

**Endpoint y objeto Zigbee**

```cpp
#define COUNTER_ENDPOINT_NUMBER 10
ZigbeeTempSensor zbCounter = ZigbeeTempSensor(COUNTER_ENDPOINT_NUMBER);
```

`zbCounter` es el objeto que representa el clúster de temperatura en el
endpoint 10 de este dispositivo. El número de endpoint es una elección
libre entre 1 y 254; no necesita coincidir con nada del lado del
coordinador.

**El contador y su tarea periódica**

```cpp
static uint32_t counterValue = 0;

static void counter_value_update(void *arg) {
  for (;;) {
    counterValue++;
    zbCounter.setTemperature((float)counterValue);
    delay(1000);
  }
}
```

`counter_value_update` corre en un **task de FreeRTOS** aparte del
`loop()` principal. Cada segundo incrementa `counterValue` y actualiza el
valor interno del clúster con `setTemperature()`. Importante:
`setTemperature()` solo actualiza el valor en memoria, no lo envía por
la red por sí solo — eso lo hace el mecanismo de *reporting*.

**`setup()`: inicialización y unión a la red**

```cpp
zbCounter.setMinMaxValue(0, 10000);
zbCounter.setTolerance(1);
Zigbee.addEndpoint(&zbCounter);
Zigbee.begin();
while (!Zigbee.connected()) { ... }
xTaskCreate(counter_value_update, ...);
zbCounter.setReporting(1, 0, 1);
```

En orden: se declara el rango del "sensor" (recuerda el límite real de
±327 explicado en la sección 4), se registra el endpoint, se arranca el
stack Zigbee como end device, se espera a conectarse a un coordinador
con la red abierta, se lanza la tarea del contador y finalmente se
configura el reporting con `(min_interval=1, max_interval=0, delta=1)`
— en la práctica, esto envía un reporte cada segundo, porque el valor
siempre cambia en 1.

**`loop()`: botón de factory reset y reporte manual**

El `loop()` no toca el contador (eso ya lo hace el task); solo vigila el
botón BOOT:
- Pulsación corta → fuerza un reporte inmediato con
  `reportTemperature()`.
- Pulsación larga (3+ segundos) → borra la configuración Zigbee del
  dispositivo con `factoryReset()` y reinicia.

## 6. Parte 2 — El Coordinador (la placa que recibe)

### 6.1 Configuración en Arduino IDE

En el menú `Tools`, para la **otra** placa:

| Opción | Valor |
|---|---|
| Board | Tu placa ESP32-C6 |
| Zigbee mode | `Zigbee ZCZR (coordinator/router)` |
| Partition Scheme | `Zigbee 4MB with spiffs` (o el esquema Zigbee equivalente) |
| Port | El puerto COM/tty de esta placa |

### 6.2 Código completo

```cpp
// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ZIGBEE_MODE_ZCZR
#error "Zigbee coordinator mode is not selected in Tools->Zigbee mode"
#endif

#include "Zigbee.h"

/* Configuración del endpoint del coordinador */
#define THERMOSTAT_ENDPOINT_NUMBER 1
uint8_t button = BOOT_PIN;

ZigbeeThermostat zbThermostat = ZigbeeThermostat(THERMOSTAT_ENDPOINT_NUMBER);

// Último valor de contador recibido y su rango configurado en el end device
float counterValue = 0;
float counter_min = 0;
float counter_max = 0;

/****************** Manejo del contador recibido *******************/
void receiveCounterValue(float value) {
  Serial.printf("Contador recibido: %.0f\n", value);
  counterValue = value;
}

void receiveCounterConfig(float min_val, float max_val, float tolerance) {
  Serial.printf("Config del end device: min %.0f, max %.0f\n", min_val, max_val);
  counter_min = min_val;
  counter_max = max_val;
}

/********************* Arduino functions **************************/
void setup() {
  Serial.begin(115200);
  pinMode(button, INPUT_PULLUP);

  // Callbacks (nombres de la API "vieja", con la errata "Recieve")
  zbThermostat.onTempRecieve(receiveCounterValue);
  zbThermostat.onConfigRecieve(receiveCounterConfig);

  zbThermostat.setManufacturerAndModel("Espressif", "ZigbeeCounterCoordinator");

  Zigbee.addEndpoint(&zbThermostat);

  // Deja la red abierta 180s tras arrancar, para que el end device se pueda unir
  Zigbee.setRebootOpenNetwork(180);

  Serial.println("Starting Zigbee...");
  if (!Zigbee.begin(ZIGBEE_COORDINATOR)) {
    Serial.println("Zigbee failed to start!");
    Serial.println("Rebooting...");
    ESP.restart();
  }

  Serial.println("Esperando a que el end device se enlace...");
  while (!zbThermostat.bound()) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("End device enlazado!");

  // Pide la configuración (rango) al end device enlazado
  zbThermostat.getSensorSettings();
}

void loop() {
  // Botón: fuerza que el end device reporte cada vez que el contador cambie en 1
  if (digitalRead(button) == LOW) {
    while (digitalRead(button) == LOW) {
      delay(50);
    }
    Serial.println("Configurando reporte del end device...");
    zbThermostat.setTemperatureReporting(0, 10, 1);
  }

  // Muestra el último valor recibido cada 10 segundos
  static uint32_t last_print = 0;
  if (millis() - last_print > 10000) {
    last_print = millis();
    Serial.printf("Último contador recibido: %.0f (rango %.0f - %.0f)\n", counterValue, counter_min, counter_max);
  }
}
```

### 6.3 Explicación paso a paso

**Verificación del modo Zigbee**

Igual que en el end device, pero exigiendo `ZIGBEE_MODE_ZCZR`
(coordinador/router) en vez de `ZIGBEE_MODE_ED`.

**Endpoint, objeto y variables de estado**

```cpp
ZigbeeThermostat zbThermostat = ZigbeeThermostat(THERMOSTAT_ENDPOINT_NUMBER);
float counterValue = 0;
float counter_min = 0;
float counter_max = 0;
```

`zbThermostat` es el objeto "cliente" (ver sección 4) que se encarga de
descubrir y enlazarse a sensores de temperatura en la red. Las tres
variables globales guardan el último dato recibido para poder
consultarlo en cualquier parte del código, incluso fuera de los
callbacks.

**Callbacks: qué hacer cuando llega un dato**

```cpp
void receiveCounterValue(float value) { ... }
void receiveCounterConfig(float min_val, float max_val, float tolerance) { ... }
```

Estas funciones **no las llamas tú**: se las "entregas" a la librería en
`setup()`, y ella las ejecuta automáticamente cuando:
- `receiveCounterValue`: llega un nuevo reporte de contador.
- `receiveCounterConfig`: llega la configuración (rango) que el end
  device declaró con `setMinMaxValue`.

**`setup()`: registrar callbacks, formar la red y enlazarse**

```cpp
zbThermostat.onTempRecieve(receiveCounterValue);
zbThermostat.onConfigRecieve(receiveCounterConfig);
Zigbee.addEndpoint(&zbThermostat);
Zigbee.setRebootOpenNetwork(180);
Zigbee.begin(ZIGBEE_COORDINATOR);
while (!zbThermostat.bound()) { ... }
zbThermostat.getSensorSettings();
```

En orden: se registran los callbacks, se agrega el endpoint, se deja la
red abierta 180 segundos para que el end device pueda unirse, se forma
la red como coordinador, y se espera (de forma bloqueante) a que el
proceso de *bind* con el end device se complete solo — la librería se
encarga de todo el descubrimiento. Una vez enlazado, se pide
explícitamente la configuración del sensor remoto.

**`loop()`: reconfigurar el reporte y mostrar el último valor**

- Botón BOOT → le pide al end device, de forma remota, que reporte al
  menos cada 10 segundos o antes si el valor cambia en 1
  (`setTemperatureReporting(0, 10, 1)`), útil para confirmar que el
  enlace funciona en ambos sentidos.
- Cada 10 segundos → imprime el último valor recibido, sin esperar a que
  llegue uno nuevo (usa lo que quedó guardado en `counterValue` por el
  callback).

### 6.4 Nota sobre nombres de la API

La librería Zigbee de Arduino-ESP32 renombró algunos métodos de
`ZigbeeThermostat` entre versiones:

| Versión usada en esta práctica | Versión más reciente |
|---|---|
| `onTempRecieve(cb)` | `onTempReceive(cb)` / `onTempReceiveWithSource(cb)` |
| `onConfigRecieve(cb)` | `onTempConfigReceive(cb)` |
| `getSensorSettings()` | `getTemperatureSettings(endpoint, addr)` |

Si tu compilador marca error de `has no member named...`, revisa qué
nombre alternativo te sugiere y ajusta el código — es señal de que tu
core tiene una versión distinta de la librería.

## 7. Poniendo todo a prueba

1. Flashea el **coordinador** (sección 7) en una placa.
2. Flashea el **end device** (sección 6) en la otra placa.
3. Enciende primero el coordinador (para que la red quede abierta) y
   luego el end device.
4. Abre el Monitor Serial del coordinador a 115200 baudios. Deberías ver
   algo así:

   ```
   Starting Zigbee...
   Esperando a que el end device se enlace...
   .........
   End device enlazado!
   Config del end device: min 0, max 10000
   Contador recibido: 1
   Contador recibido: 2
   Contador recibido: 3
   ...
   Último contador recibido: 7 (rango 0 - 10000)
   ```

5. Prueba el botón BOOT en cada placa:
   - En el end device: pulsación corta fuerza un reporte inmediato;
     pulsación larga (3+s) borra la red Zigbee guardada.
   - En el coordinador: pulsación corta reconfigura el intervalo de
     reporte del end device.

## 8. Repaso de conceptos aplicados

| Concepto (sección 3) | Dónde lo viste en el código |
|---|---|
| Roles (coordinador / end device) | `Zigbee.begin(ZIGBEE_COORDINATOR)` vs `Zigbee.begin()` en modo ED |
| Endpoint | `COUNTER_ENDPOINT_NUMBER` / `THERMOSTAT_ENDPOINT_NUMBER` |
| Clúster | El clúster de Temperature Measurement, vía `ZigbeeTempSensor` / `ZigbeeThermostat` |
| Bind | `while (!zbThermostat.bound())` |
| Reporting | `setReporting(...)` en el end device, `setTemperatureReporting(...)` desde el coordinador |

## 9. Limitaciones y siguientes pasos

- El valor está limitado a un rango de aproximadamente -273 a 327, por
  estar viajando "disfrazado" de temperatura (ver sección 4).
- No existe todavía, en esta librería, una forma tan madura de hacer lo
  mismo con un clúster más apropiado (como Analog Input) del lado del
  coordinador.

Como siguiente paso, una vez entendidos estos conceptos, podrías intentar:
- Reemplazar el contador por una lectura real de un sensor (por ejemplo,
  temperatura del chip con `temperatureRead()`, como en el ejemplo
  original de Espressif).
- Agregar un segundo end device y observar cómo el coordinador puede
  enlazarse a varios sensores a la vez.