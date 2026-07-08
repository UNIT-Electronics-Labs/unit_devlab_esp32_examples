# Wi-Fi y Bluetooth LE en ESP32-C6

El ESP32-C6 integra **Wi-Fi 6** (802.11ax) y **Bluetooth 5.3** para conectividad inalámbrica.

## Wi-Fi en ESP32-C6

### Características

| Característica | Especificación |
|----------------|----------------|
| Estándar | **Wi-Fi 6** (802.11ax) compatible con b/g/n |
| Frecuencia | 2.4 GHz |
| Modos | Station, Access Point, Station+AP |
| Seguridad | WPA3, WPA2, WPA, WEP |
| Alcance | ~100m en exterior, ~50m en interior |
| Velocidad | Hasta 20 Mbps |

### Modos de operación

#### 1. Station Mode (STA)
ESP32-C6 se conecta a un router Wi-Fi existente.

#### 2. Access Point Mode (AP)
ESP32-C6 actúa como punto de acceso Wi-Fi.

#### 3. Station + AP Mode
Ambos modos simultáneamente.

## Configurar Wi-Fi Station

### Conexión básica

```c
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#define WIFI_SSID      "TuRedWiFi"
#define WIFI_PASS      "TuContraseña"

static const char *TAG = "WIFI";

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Desconectado, reintentando...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "IP obtenida: " IPSTR, IP2STR(&event->ip_info.ip));
    }
}

void wifi_init_sta(void)
{
    // Inicializar NVS
    ESP_ERROR_CHECK(nvs_flash_init());
    
    // Inicializar TCP/IP stack
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    
    // Configuración Wi-Fi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    // Registrar event handlers
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, 
                                               &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, 
                                               &wifi_event_handler, NULL));
    
    // Configurar SSID y contraseña
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    ESP_LOGI(TAG, "Wi-Fi inicializado en modo Station");
}

void app_main(void)
{
    wifi_init_sta();
}
```

### Escaneo de redes

```c
void wifi_scan(void)
{
    uint16_t number = 10;
    wifi_ap_record_t ap_info[10];
    uint16_t ap_count = 0;
    
    esp_wifi_scan_start(NULL, true);
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
    
    ESP_LOGI(TAG, "Redes encontradas: %d", ap_count);
    
    for (int i = 0; i < number; i++) {
        ESP_LOGI(TAG, "SSID: %s, RSSI: %d, Canal: %d", 
                ap_info[i].ssid, ap_info[i].rssi, ap_info[i].primary);
    }
}
```

### Obtener estado de conexión

```c
#include "esp_wifi.h"

bool wifi_is_connected(void)
{
    wifi_ap_record_t ap_info;
    esp_err_t ret = esp_wifi_sta_get_ap_info(&ap_info);
    return (ret == ESP_OK);
}

int8_t wifi_get_rssi(void)
{
    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
        return ap_info.rssi;
    }
    return -100;
}
```

## HTTP Client (Peticiones web)

```c
#include "esp_http_client.h"

esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "Datos recibidos: %.*s", evt->data_len, (char *)evt->data);
            break;
        default:
            break;
    }
    return ESP_OK;
}

void http_get_request(void)
{
    esp_http_client_config_t config = {
        .url = "http://httpbin.org/get",
        .event_handler = http_event_handler,
    };
    
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);
    
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET Status = %d", 
                esp_http_client_get_status_code(client));
    }
    
    esp_http_client_cleanup(client);
}

void http_post_json(const char *json_data)
{
    esp_http_client_config_t config = {
        .url = "http://api.example.com/sensor",
        .method = HTTP_METHOD_POST,
    };
    
    esp_http_client_handle_t client = esp_http_client_init(&config);
    
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, json_data, strlen(json_data));
    
    esp_err_t err = esp_http_client_perform(client);
    
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Datos enviados correctamente");
    }
    
    esp_http_client_cleanup(client);
}
```

## MQTT Client

```c
#include "mqtt_client.h"

static esp_mqtt_client_handle_t mqtt_client;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT conectado");
            esp_mqtt_client_subscribe(mqtt_client, "sensor/temperature", 0);
            break;
            
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "Mensaje recibido: %.*s", 
                    event->data_len, event->data);
            break;
            
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT desconectado");
            break;
            
        default:
            break;
    }
}

void mqtt_init(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://broker.hivemq.com:1883",
        .credentials.username = "usuario",
        .credentials.authentication.password = "contraseña",
    };
    
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, 
                                   mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

void mqtt_publish(const char *topic, const char *data)
{
    esp_mqtt_client_publish(mqtt_client, topic, data, 0, 1, 0);
}
```

## Bluetooth LE en ESP32-C6

### Características

| Característica | Especificación |
|----------------|----------------|
| Estándar | **Bluetooth 5.3** (LE) |
| Velocidad | Hasta 2 Mbps |
| Alcance | ~100m |
| Consumo | Ultra bajo (<1 mA en modo advertising) |
| Roles | Central, Peripheral, Broadcaster, Observer |

### Conceptos BLE

#### GAP (Generic Access Profile)
Define roles y modos de descubrimiento:
- **Peripheral**: dispositivo que anuncia datos (sensor, wearable)
- **Central**: dispositivo que escanea y se conecta (smartphone, gateway)

#### GATT (Generic Attribute Profile)
Define estructura de servicios y características:
- **Service**: colección de características (ej: Heart Rate Service)
- **Characteristic**: un dato (ej: Heart Rate Measurement)

### BLE Peripheral (Servidor)

```c
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "esp_log.h"

#define GATTS_SERVICE_UUID   0x00FF
#define GATTS_CHAR_UUID      0xFF01

static const char *TAG = "BLE_SERVER";

static uint8_t adv_data[] = {
    0x02, 0x01, 0x06,  // Flags
    0x03, 0x03, 0xFF, 0x00,  // Service UUID
};

static void gap_event_handler(esp_gap_ble_cb_event_t event, 
                             esp_ble_gap_cb_param_t *param)
{
    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            ESP_LOGI(TAG, "Advertising iniciado");
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
            ESP_LOGI(TAG, "GATT servidor registrado");
            esp_ble_gap_set_device_name("ESP32-C6");
            esp_ble_gap_config_adv_data(adv_data, sizeof(adv_data));
            break;
            
        case ESP_GATTS_READ_EVT:
            ESP_LOGI(TAG, "Lectura de característica");
            break;
            
        case ESP_GATTS_WRITE_EVT:
            ESP_LOGI(TAG, "Escritura: %.*s", 
                    param->write.len, param->write.value);
            break;
            
        default:
            break;
    }
}

void ble_init(void)
{
    // Inicializar controlador BT
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));
    
    // Inicializar Bluedroid
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());
    
    // Registrar callbacks
    esp_ble_gap_register_callback(gap_event_handler);
    esp_ble_gatts_register_callback(gatts_event_handler);
    esp_ble_gatts_app_register(0);
}
```

### BLE Central (Cliente)

```c
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"

static void gattc_event_handler(esp_gattc_cb_event_t event,
                                esp_gatt_if_t gattc_if,
                                esp_ble_gattc_cb_param_t *param)
{
    switch (event) {
        case ESP_GATTC_CONNECT_EVT:
            ESP_LOGI(TAG, "Conectado al servidor BLE");
            esp_ble_gattc_search_service(gattc_if, param->connect.conn_id, NULL);
            break;
            
        case ESP_GATTC_SEARCH_RES_EVT:
            ESP_LOGI(TAG, "Servicio encontrado: UUID %04X", 
                    param->search_res.srvc_id.uuid.uuid.uuid16);
            break;
            
        case ESP_GATTC_READ_CHAR_EVT:
            ESP_LOGI(TAG, "Dato leído: %.*s", 
                    param->read.value_len, param->read.value);
            break;
            
        default:
            break;
    }
}

void ble_scan_start(void)
{
    esp_ble_gap_start_scanning(30);  // Escanear 30 segundos
}
```

### BLE Beacon (iBeacon)

```c
static uint8_t ibeacon_adv_data[] = {
    0x02, 0x01, 0x06,  // Flags
    0x1A, 0xFF,        // Manufacturer data
    0x4C, 0x00,        // Apple Company ID
    0x02, 0x15,        // iBeacon type
    // UUID (16 bytes)
    0xFD, 0xA5, 0x06, 0x93, 0xA4, 0xE2, 0x4F, 0xB1,
    0xAF, 0xCF, 0xC6, 0xEB, 0x07, 0x64, 0x78, 0x25,
    0x00, 0x01,        // Major
    0x00, 0x02,        // Minor
    0xC5               // TX Power
};

void ibeacon_init(void)
{
    esp_ble_gap_config_adv_data_raw(ibeacon_adv_data, sizeof(ibeacon_adv_data));
}
```

## Consumo de energía

### Wi-Fi
- **Activo**: ~100 mA
- **Modem sleep**: ~20 mA
- **Light sleep**: ~1 mA
- **Deep sleep**: ~10 µA

### Bluetooth LE
- **Advertising**: ~0.5 mA
- **Conectado**: ~10-30 mA
- **Deep sleep**: ~10 µA

### Modos de bajo consumo

```c
#include "esp_sleep.h"

void enter_deep_sleep(uint64_t sleep_time_us)
{
    ESP_LOGI(TAG, "Entrando en deep sleep por %lld segundos", 
            sleep_time_us / 1000000);
    
    esp_sleep_enable_timer_wakeup(sleep_time_us);
    esp_deep_sleep_start();
}

void enter_light_sleep(uint64_t sleep_time_us)
{
    esp_sleep_enable_timer_wakeup(sleep_time_us);
    esp_light_sleep_start();
}
```

## Coexistencia Wi-Fi y BLE

ESP32-C6 puede usar Wi-Fi y BLE simultáneamente:

```c
// Inicializar Wi-Fi
wifi_init_sta();

// Inicializar BLE
ble_init();

// Ambos funcionan al mismo tiempo
```

::: warning Rendimiento
Usar Wi-Fi y BLE simultáneamente reduce ligeramente el throughput de cada uno.
:::

## Próximos pasos

- [Plataformas IoT](./iot-platforms.md)
- [Lab 4: Publicación Wi-Fi](../examples/lab04-wifi.md)
- [Lab 5: Bluetooth LE](../examples/lab05-ble.md)

## Referencias

- [ESP32-C6 Wi-Fi Driver](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/network/esp_wifi.html)
- [ESP32-C6 Bluetooth LE](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/bluetooth/index.html)
- [ESP-IDF HTTP Client](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/protocols/esp_http_client.html)
- [ESP-MQTT](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/protocols/mqtt.html)
