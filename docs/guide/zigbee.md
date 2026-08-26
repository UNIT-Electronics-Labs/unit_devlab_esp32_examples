# Zigbee con la UNIT Pulsar ESP32-C6

## 1. Introducción

Zigbee es un protocolo de comunicación inalámbrica de bajo consumo basado en el estándar **IEEE 802.15.4.** Está diseñado principalmente para aplicaciones de IoT, automatización, sensores y dispositivos embebidos que requieren comunicación inalámbrica con bajo consumo energético y la posibilidad de formar redes con múltiples dispositivos.

A diferencia de tecnologías como **Wi-Fi**, Zigbee está orientado a transmitir pequeñas cantidades de información y permite crear redes con topologías estrella, árbol y malla.

En este tutorial revisaremos el protocolo **Zigbee** utilizando empleando dos entornos de desarrollo:

- Arduino IDE.
- Espressif ESP-IDF.

El objetivo es comprender desde los conceptos fundamentales de Zigbee hasta la comunicación entre múltiples dispositivos ESP32-C6.


## 2. ESP32-C6 y Zigbee

El ESP32-C6 tiene soporte para IEEE 802.15.4 permite utilizar tecnologías como:

- Zigbee.
- Thread.

Esto hace que el ESP32-C6 pueda utilizarse directamente como nodo Zigbee sin necesidad de agregar un transceptor Zigbee externo.

## 3. Conceptos fundamentales

Es importante comprender algunos conceptos utilizados dentro de una red Zigbee.

### 3.1 Coordinator

El Coordinator es el dispositivo encargado de crear la red Zigbee.

En una red solamente puede existir un Coordinator.

Sus principales funciones son:

- Crear la red.
- Seleccionar el canal Zigbee.
- Asignar parámetros de red.
- Permitir que otros dispositivos se unan.
- Inicializar la estructura de la red.

### 3.2 Router

Un Router puede comunicarse con otros dispositivos y además retransmitir mensajes.

Esto permite extender el alcance de la red.

```text
Coordinator
     │
     │
   Router
   /    \
Node    Node
```

### 3.3 End Device

Un End Device representa normalmente un dispositivo final como:

- Sensor.
- Interruptor.
- Actuador.
- Medidor.
- Dispositivo alimentado por batería.

Los End Devices no retransmiten paquetes de otros dispositivos.

### 3.4 Endpoints

Un Endpoint puede entenderse como una interfaz lógica dentro de un dispositivo Zigbee.

Por ejemplo:

```text
ESP32-C6
│
├── Endpoint 1
│   └── Sensor de temperatura
│
├── Endpoint 2
│   └── Interruptor
│
└── Endpoint 3
    └── Sensor ADC
```

Un mismo ESP32-C6 puede implementar varios dispositivos lógicos utilizando diferentes Endpoints.

### 3.5 Clusters

Los Clusters agrupan funcionalidades relacionadas.

Algunos ejemplos son:

| Cluster                 | Función                            |
|-------------------------|------------------------------------|
| Basic                   | Información básica del dispositivo |
| Identify                | Identificación                     |
| On/Off                  | Encendido y apagado                |
| Temperature Measurement |	Temperatura                        |
| Analog Input            |	Entrada analógica                  |

## 4. Redes

Zigbee permite diferentes formas de organizar los dispositivos.

### 4.1 Estrella

```text
       End Device
           │
           │
End ── Coordinator ── End
           │
           │
       End Device
```

Todos los dispositivos se comunican directamente con el Coordinator.

### 4.2 Árbol

```text
          Coordinator
          /         \
      Router       Router
      /   \         /   \
    End   End     End   End
```

### 4.3 Malla

```text
        Coordinator
        /         \
    Router ───── Router
      │   \       / │
      │    \     /  │
     End    Router   End
```

La topología de malla permite que existan diferentes rutas para transportar la información.

## 5. Primeros pasos

Para este ejemplo crearemos un **coordinator** y un **end device**. 

#### Requisitos

1. Dos UNIT Pulsar ESP32-C6.
2. Un cable USB para cada ESP32-C6.

### Arduino IDE

Arduino permite realizar una implementación más sencilla y abstrae gran parte de la configuración interna del stack Zigbee.

### ESP-IDF

ESP-IDF permite trabajar directamente con las herramientas oficiales de Espressif y proporciona mayor control sobre:
