# Lab 5: Comunicación Bluetooth LE

En esta práctica aprenderás a usar Bluetooth Low Energy para comunicarte con dispositivos móviles y crear aplicaciones IoT de bajo consumo.

## Objetivos

- Configurar Bluetooth LE en ESP32-C6
- Crear un servidor GATT (Peripheral)
- Leer/escribir características BLE
- Conectar con smartphone
- Implementar beacon iBeacon
- Bajo consumo de energía

## Materiales

- Placa Pulsar C6 (ESP32-C6)
- TTP223B sensor capacitivo touch
- Smartphone con BLE (Android/iOS)
- App BLE (nRF Connect, LightBlue)

## Teoría: BLE Basics

### Roles BLE

- **Peripheral (Servidor)**: anuncia y proporciona datos (ESP32-C6)
- **Central (Cliente)**: escanea y se conecta (Smartphone)

### Estructura GATT

```
Servicio (Service)
└── Característica 1 (Characteristic)
    ├── Valor (Value)
    ├── Descriptor
    └── Propiedades (Read, Write, Notify)
└── Característica 2
```

## Código base Arduino

Este sketch crea un servidor BLE sencillo que publica el estado del TTP223B.
Desde nRF Connect o LightBlue busca `PulsarC6-BLE` y lee la característica.

```cpp
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

const char* DEVICE_NAME = "PulsarC6-BLE";
const char* SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
const char* TOUCH_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
const int TOUCH_PIN = 5;

BLECharacteristic* touchCharacteristic;

void setup() {
  Serial.begin(115200);
  pinMode(TOUCH_PIN, INPUT);

  BLEDevice::init(DEVICE_NAME);
  BLEServer* server = BLEDevice::createServer();
  BLEService* service = server->createService(SERVICE_UUID);

  touchCharacteristic = service->createCharacteristic(
    TOUCH_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );
  touchCharacteristic->addDescriptor(new BLE2902());
  touchCharacteristic->setValue("released");

  service->start();
  BLEAdvertising* advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  advertising->start();

  Serial.println("BLE listo. Busca PulsarC6-BLE desde el telefono.");
}

void loop() {
  bool touched = digitalRead(TOUCH_PIN) == HIGH;
  const char* value = touched ? "touched" : "released";

  touchCharacteristic->setValue(value);
  touchCharacteristic->notify();

  Serial.println(value);
  delay(250);
}
```

### Conexiones TTP223B

```
TTP223B
━━━━━━━━━━━━━━━━━━━━
OUT  ───── GPIO5
VCC  ───── 3.3V
GND  ───── GND
```

## Referencia avanzada ESP-IDF

Si necesitas portar esta practica a ESP-IDF, usa NimBLE o Bluedroid y publica el mismo estado del TTP223B. Para el curso base, usa el sketch Arduino anterior.

## Ejercicios

### Ejercicio 1: Sensor completo
Crea un servidor BLE con dos características: estado del TTP223B y temperatura del TMP235.

### Ejercicio 2: App móvil
Usa Flutter o React Native para crear una app que lea los datos BLE.

### Ejercicio 3: Seguridad
Implementa pairing y bonding para conexiones seguras.

### Ejercicio 4: Múltiples clientes
Permite que varios smartphones se conecten simultáneamente.

## Comparación Wi-Fi vs BLE

| Característica | Wi-Fi | BLE |
|----------------|-------|-----|
| **Alcance** | 50-100m | 10-50m |
| **Velocidad** | Hasta 20 Mbps | Hasta 2 Mbps |
| **Consumo** | ~100 mA activo | ~10 mA activo, <1 mA advertising |
| **Conexión** | Requiere router | Directo dispositivo-dispositivo |
| **Uso típico** | Cloud, internet | Wearables, sensores |

## Apps útiles para BLE

| App | Plataforma | Función |
|-----|------------|---------|
| **nRF Connect** | Android/iOS | Scanner, GATT explorer |
| **LightBlue** | iOS | Scanner BLE, simple |
| **BLE Scanner** | Android | Scanner, debugging |
| **Serial Bluetooth Terminal** | Android | Comunicación UART over BLE |

## Solución de problemas

| Problema | Solución |
|----------|----------|
| No se detecta BLE | Verificar que smartphone tiene BLE, activar Bluetooth |
| Conexión falla | Reducir distancia, evitar interferencias |
| No puede escribir | Verificar permisos de característica |
| Desconexiones frecuentes | Aumentar intervalo de conexión |

## Seguridad BLE

Para prácticas de laboratorio puedes usar BLE sin pairing. Para un proyecto con
datos sensibles, revisa las opciones de pairing/bonding de la librería BLE que
uses en Arduino y evita publicar datos privados en características abiertas.

## Próximos pasos

- Combina Wi-Fi y BLE para gateway IoT
- Explora BLE Mesh para redes de sensores
- Implementa OTA updates via BLE

## Referencias

- [Arduino BLE library](https://docs.arduino.cc/libraries/arduinoble/)
- [nRF Connect App](https://www.nordicsemi.com/Products/Development-tools/nrf-connect-for-mobile)
- [BLE GATT Specification](https://www.bluetooth.com/specifications/specs/core-specification-5-3/)
- [Bluetooth SIG](https://www.bluetooth.com/)
- [ESP32-C6 BLE](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/bluetooth/esp_gap_ble.html) - referencia avanzada
# Lab 5: Comunicación Bluetooth LE

En esta práctica aprenderás a usar Bluetooth Low Energy para comunicarte con dispositivos móviles y crear aplicaciones IoT de bajo consumo.

## Objetivos

- Configurar Bluetooth LE en ESP32-C6
- Crear un servidor GATT (Peripheral)
- Leer/escribir características BLE
- Conectar con smartphone
- Implementar beacon iBeacon
- Bajo consumo de energía

## Materiales

- Placa Pulsar C6 (ESP32-C6)
- TTP223B sensor capacitivo touch
- Smartphone con BLE (Android/iOS)
- App BLE (nRF Connect, LightBlue)

## Teoría: BLE Basics

### Roles BLE

- **Peripheral (Servidor)**: anuncia y proporciona datos (ESP32-C6)
- **Central (Cliente)**: escanea y se conecta (Smartphone)

### Estructura GATT

```
Servicio (Service)
└── Característica 1 (Characteristic)
    ├── Valor (Value)
    ├── Descriptor
    └── Propiedades (Read, Write, Notify)
└── Característica 2
```

## Código base Arduino

Este sketch crea un servidor BLE sencillo que publica el estado del TTP223B.
Desde nRF Connect o LightBlue busca `PulsarC6-BLE` y lee la característica.

```cpp
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

const char* DEVICE_NAME = "PulsarC6-BLE";
const char* SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
const char* TOUCH_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
const int TOUCH_PIN = 5;

BLECharacteristic* touchCharacteristic;

void setup() {
  Serial.begin(115200);
  pinMode(TOUCH_PIN, INPUT);

  BLEDevice::init(DEVICE_NAME);
  BLEServer* server = BLEDevice::createServer();
  BLEService* service = server->createService(SERVICE_UUID);

  touchCharacteristic = service->createCharacteristic(
    TOUCH_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );
  touchCharacteristic->addDescriptor(new BLE2902());
  touchCharacteristic->setValue("released");

  service->start();
  BLEAdvertising* advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  advertising->start();

  Serial.println("BLE listo. Busca PulsarC6-BLE desde el telefono.");
}

void loop() {
  bool touched = digitalRead(TOUCH_PIN) == HIGH;
  const char* value = touched ? "touched" : "released";

  touchCharacteristic->setValue(value);
  touchCharacteristic->notify();

  Serial.println(value);
  delay(250);
}
```

### Conexiones TTP223B

```
TTP223B
━━━━━━━━━━━━━━━━━━━━
OUT  ───── GPIO5
VCC  ───── 3.3V
GND  ───── GND
```

## Referencia avanzada ESP-IDF

Si necesitas portar esta practica a ESP-IDF, usa NimBLE o Bluedroid y publica el mismo estado del TTP223B. Para el curso base, usa el sketch Arduino anterior.

## Ejercicios

### Ejercicio 1: Sensor completo
Crea un servidor BLE con dos características: estado del TTP223B y temperatura del TMP235.

### Ejercicio 2: App móvil
Usa Flutter o React Native para crear una app que lea los datos BLE.

### Ejercicio 3: Seguridad
Implementa pairing y bonding para conexiones seguras.

### Ejercicio 4: Múltiples clientes
Permite que varios smartphones se conecten simultáneamente.

## Comparación Wi-Fi vs BLE

| Característica | Wi-Fi | BLE |
|----------------|-------|-----|
| **Alcance** | 50-100m | 10-50m |
| **Velocidad** | Hasta 20 Mbps | Hasta 2 Mbps |
| **Consumo** | ~100 mA activo | ~10 mA activo, <1 mA advertising |
| **Conexión** | Requiere router | Directo dispositivo-dispositivo |
| **Uso típico** | Cloud, internet | Wearables, sensores |

## Apps útiles para BLE

| App | Plataforma | Función |
|-----|------------|---------|
| **nRF Connect** | Android/iOS | Scanner, GATT explorer |
| **LightBlue** | iOS | Scanner BLE, simple |
| **BLE Scanner** | Android | Scanner, debugging |
| **Serial Bluetooth Terminal** | Android | Comunicación UART over BLE |

## Solución de problemas

| Problema | Solución |
|----------|----------|
| No se detecta BLE | Verificar que smartphone tiene BLE, activar Bluetooth |
| Conexión falla | Reducir distancia, evitar interferencias |
| No puede escribir | Verificar permisos de característica |
| Desconexiones frecuentes | Aumentar intervalo de conexión |

## Seguridad BLE

Para prácticas de laboratorio puedes usar BLE sin pairing. Para un proyecto con
datos sensibles, revisa las opciones de pairing/bonding de la librería BLE que
uses en Arduino y evita publicar datos privados en características abiertas.

## Próximos pasos

- Combina Wi-Fi y BLE para gateway IoT
- Explora BLE Mesh para redes de sensores
- Implementa OTA updates via BLE

## Referencias

- [Arduino BLE library](https://docs.arduino.cc/libraries/arduinoble/)
- [nRF Connect App](https://www.nordicsemi.com/Products/Development-tools/nrf-connect-for-mobile)
- [BLE GATT Specification](https://www.bluetooth.com/specifications/specs/core-specification-5-3/)
- [Bluetooth SIG](https://www.bluetooth.com/)
- [ESP32-C6 BLE](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/bluetooth/esp_gap_ble.html) - referencia avanzada
