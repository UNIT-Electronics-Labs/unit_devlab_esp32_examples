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
- Smartphone con BLE (Android/iOS)
- App BLE (nRF Connect, LightBlue)
- Sensor BME280 (opcional)

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

Este sketch crea un servidor BLE sencillo con una característica de lectura.
Desde nRF Connect o LightBlue busca `PulsarC6-BLE` y lee la característica.

```cpp
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

const char* DEVICE_NAME = "PulsarC6-BLE";
const char* SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
const char* TEMP_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

BLECharacteristic* tempCharacteristic;

void setup() {
  Serial.begin(115200);

  BLEDevice::init(DEVICE_NAME);
  BLEServer* server = BLEDevice::createServer();
  BLEService* service = server->createService(SERVICE_UUID);

  tempCharacteristic = service->createCharacteristic(
    TEMP_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );
  tempCharacteristic->addDescriptor(new BLE2902());
  tempCharacteristic->setValue("24.5 C");

  service->start();
  BLEAdvertising* advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  advertising->start();

  Serial.println("BLE listo. Busca PulsarC6-BLE desde el telefono.");
}

void loop() {
  static float temperature = 24.5;
  temperature += 0.1;

  String value = String(temperature, 1) + " C";
  tempCharacteristic->setValue(value.c_str());
  tempCharacteristic->notify();

  Serial.println(value);
  delay(2000);
}
```

## Referencia avanzada ESP-IDF

## Práctica 1: BLE Beacon (Advertising)

El modo más simple: solo transmite datos sin conexión.

### Código: iBeacon

```c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_bt_main.h"
#include "esp_log.h"

static const char *TAG = "IBEACON";

// iBeacon advertising data
static uint8_t ibeacon_adv_data[] = {
    0x02, 0x01, 0x06,  // Flags
    0x1A, 0xFF,        // Manufacturer data
    0x4C, 0x00,        // Apple Company ID
    0x02, 0x15,        // iBeacon type
    // UUID (16 bytes) - Personaliza esto
    0xFD, 0xA5, 0x06, 0x93, 0xA4, 0xE2, 0x4F, 0xB1,
    0xAF, 0xCF, 0xC6, 0xEB, 0x07, 0x64, 0x78, 0x25,
    0x00, 0x01,        // Major (0x0001 = 1)
    0x00, 0x02,        // Minor (0x0002 = 2)
    0xC5               // TX Power (-59 dBm)
};

static esp_ble_adv_params_t adv_params = {
    .adv_int_min = 0x20,
    .adv_int_max = 0x40,
    .adv_type = ADV_TYPE_NONCONN_IND,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static void gap_event_handler(esp_gap_ble_cb_event_t event, 
                             esp_ble_gap_cb_param_t *param)
{
    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
            esp_ble_gap_start_advertising(&adv_params);
            break;
            
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            if (param->adv_start_cmpl.status == ESP_BT_STATUS_SUCCESS) {
                ESP_LOGI(TAG, "iBeacon advertising iniciado");
            }
            break;
            
        default:
            break;
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));
    
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());
    
    ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_event_handler));
    ESP_ERROR_CHECK(esp_ble_gap_config_adv_data_raw(ibeacon_adv_data, 
                                                    sizeof(ibeacon_adv_data)));
    
    ESP_LOGI(TAG, "iBeacon configurado. UUID: FDA50693-A4E2-4FB1-AFCF-C6EB07647825");
}
```

### Prueba con smartphone

1. Descarga app "Beacon Scanner" (Android) o "Locate Beacon" (iOS)
2. Escanea beacons cercanos
3. Verás tu ESP32-C6 con el UUID configurado

## Práctica 2: BLE GATT Server (Lectura/Escritura)

### Código: Servidor BLE con LED control

```c
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define GATTS_TAG "GATT_SERVER"

// UUIDs personalizados
#define GATTS_SERVICE_UUID      0x00FF
#define GATTS_CHAR_LED_UUID     0xFF01
#define GATTS_CHAR_TEMP_UUID    0xFF02

#define LED_GPIO                GPIO_NUM_8

static uint8_t adv_data[] = {
    0x02, 0x01, 0x06,
    0x03, 0x03, 0xFF, 0x00
};

static uint8_t led_value = 0;
static float temperature_value = 25.5;

static uint16_t service_handle;
static uint16_t char_led_handle;
static uint16_t char_temp_handle;

static esp_ble_adv_params_t adv_params = {
    .adv_int_min = 0x20,
    .adv_int_max = 0x40,
    .adv_type = ADV_TYPE_IND,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static void gap_event_handler(esp_gap_ble_cb_event_t event, 
                             esp_ble_gap_cb_param_t *param)
{
    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            esp_ble_gap_start_advertising(&adv_params);
            break;
        default:
            break;
    }
}

static void gatts_event_handler(esp_gatts_cb_event_t event,
                                esp_gatt_if_t gatts_if,
                                esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
        case ESP_GATTS_REG_EVT:
            ESP_LOGI(GATTS_TAG, "GATT server registrado");
            
            esp_ble_gap_set_device_name("ESP32-C6 Sensor");
            esp_ble_gap_config_adv_data(&adv_data[0], sizeof(adv_data));
            
            // Crear servicio
            esp_gatt_srvc_id_t service_id;
            service_id.is_primary = true;
            service_id.id.inst_id = 0x00;
            service_id.id.uuid.len = ESP_UUID_LEN_16;
            service_id.id.uuid.uuid.uuid16 = GATTS_SERVICE_UUID;
            
            esp_ble_gatts_create_service(gatts_if, &service_id, 4);
            break;
            
        case ESP_GATTS_CREATE_EVT:
            service_handle = param->create.service_handle;
            ESP_LOGI(GATTS_TAG, "Servicio creado, handle: %d", service_handle);
            
            esp_ble_gatts_start_service(service_handle);
            
            // Agregar característica LED (Read/Write)
            esp_bt_uuid_t char_led_uuid;
            char_led_uuid.len = ESP_UUID_LEN_16;
            char_led_uuid.uuid.uuid16 = GATTS_CHAR_LED_UUID;
            
            esp_gatt_char_prop_t property = ESP_GATT_CHAR_PROP_BIT_READ | 
                                           ESP_GATT_CHAR_PROP_BIT_WRITE;
            
            esp_ble_gatts_add_char(service_handle, &char_led_uuid, 
                                  ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                                  property, NULL, NULL);
            break;
            
        case ESP_GATTS_ADD_CHAR_EVT:
            ESP_LOGI(GATTS_TAG, "Característica agregada, handle: %d", 
                    param->add_char.attr_handle);
            
            if (param->add_char.char_uuid.uuid.uuid16 == GATTS_CHAR_LED_UUID) {
                char_led_handle = param->add_char.attr_handle;
            }
            break;
            
        case ESP_GATTS_WRITE_EVT:
            ESP_LOGI(GATTS_TAG, "Escritura recibida, handle: %d, len: %d", 
                    param->write.handle, param->write.len);
            
            if (param->write.handle == char_led_handle && param->write.len == 1) {
                led_value = param->write.value[0];
                gpio_set_level(LED_GPIO, led_value);
                ESP_LOGI(GATTS_TAG, "LED: %s", led_value ? "ON" : "OFF");
            }
            break;
            
        case ESP_GATTS_READ_EVT:
            ESP_LOGI(GATTS_TAG, "Lectura solicitada, handle: %d", 
                    param->read.handle);
            
            esp_gatt_rsp_t rsp;
            memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
            rsp.attr_value.handle = param->read.handle;
            
            if (param->read.handle == char_led_handle) {
                rsp.attr_value.len = 1;
                rsp.attr_value.value[0] = led_value;
            }
            
            esp_ble_gatts_send_response(gatts_if, param->read.conn_id, 
                                       param->read.trans_id, ESP_GATT_OK, &rsp);
            break;
            
        case ESP_GATTS_CONNECT_EVT:
            ESP_LOGI(GATTS_TAG, "Cliente conectado");
            break;
            
        case ESP_GATTS_DISCONNECT_EVT:
            ESP_LOGI(GATTS_TAG, "Cliente desconectado, reiniciando advertising");
            esp_ble_gap_start_advertising(&adv_params);
            break;
            
        default:
            break;
    }
}

void app_main(void)
{
    // Configurar LED
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
    
    // Inicializar Bluetooth
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));
    
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());
    
    ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_event_handler));
    ESP_ERROR_CHECK(esp_ble_gatts_register_callback(gatts_event_handler));
    
    ESP_ERROR_CHECK(esp_ble_gatts_app_register(0));
    
    ESP_LOGI(GATTS_TAG, "Servidor BLE iniciado");
}
```

### Prueba con smartphone

1. Descarga **nRF Connect** (Android/iOS)
2. Escanea dispositivos BLE
3. Conecta a "ESP32-C6 Sensor"
4. Encuentra característica UUID 0xFF01
5. Escribe `0x01` para encender LED, `0x00` para apagar

## Práctica 3: BLE Notify (Notificaciones)

Envía datos automáticamente al cliente cuando cambian.

### Código: Sensor con notificaciones

```c
#include "esp_gatts_api.h"

#define GATTS_CHAR_TEMP_UUID 0xFF02

static uint16_t conn_id = 0;
static esp_gatt_if_t gatts_if_global = 0;
static bool is_connected = false;

// Agregar propiedad NOTIFY
esp_gatt_char_prop_t property = ESP_GATT_CHAR_PROP_BIT_READ | 
                               ESP_GATT_CHAR_PROP_BIT_NOTIFY;

static void gatts_event_handler(esp_gatts_cb_event_t event,
                                esp_gatt_if_t gatts_if,
                                esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
        case ESP_GATTS_CONNECT_EVT:
            conn_id = param->connect.conn_id;
            gatts_if_global = gatts_if;
            is_connected = true;
            ESP_LOGI(TAG, "Cliente conectado");
            break;
            
        case ESP_GATTS_DISCONNECT_EVT:
            is_connected = false;
            ESP_LOGI(TAG, "Cliente desconectado");
            esp_ble_gap_start_advertising(&adv_params);
            break;
            
        // ... resto del código ...
    }
}

void notify_temperature(float temperature)
{
    if (is_connected) {
        uint8_t data[4];
        memcpy(data, &temperature, sizeof(float));
        
        esp_ble_gatts_send_indicate(gatts_if_global, conn_id, 
                                    char_temp_handle,
                                    sizeof(data), data, false);
        
        ESP_LOGI(TAG, "Temperatura notificada: %.2f °C", temperature);
    }
}

void sensor_task(void *pvParameters)
{
    while (1) {
        float temperature = read_temperature();  // Tu función de lectura
        notify_temperature(temperature);
        
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void app_main(void)
{
    // ... inicialización BLE ...
    
    xTaskCreate(sensor_task, "sensor", 4096, NULL, 5, NULL);
}
```

## Práctica 4: BLE + Bajo Consumo

### Código: Deep Sleep con BLE Wakeup

```c
#include "esp_sleep.h"

void enter_deep_sleep_with_ble(void)
{
    ESP_LOGI(TAG, "Entrando en deep sleep...");
    
    // Configurar wakeup por timer (10 segundos)
    esp_sleep_enable_timer_wakeup(10 * 1000000);
    
    // Entrar en deep sleep
    esp_deep_sleep_start();
}

void app_main(void)
{
    // Inicializar BLE
    // ...
    
    // Anunciar por 30 segundos
    vTaskDelay(pdMS_TO_TICKS(30000));
    
    // Dormir
    enter_deep_sleep_with_ble();
}
```

## Ejercicios

### Ejercicio 1: Sensor completo
Crea servidor BLE con 3 características: temperatura, humedad, presión. Todas con notify.

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

```c
// Habilitar encriptación
esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_MITM_BOND;
esp_ble_io_cap_t iocap = ESP_IO_CAP_OUT;  // Display only
uint8_t key_size = 16;
uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;

esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));
esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
```

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
