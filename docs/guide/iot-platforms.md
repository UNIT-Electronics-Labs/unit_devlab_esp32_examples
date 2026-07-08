# Plataformas IoT

Las plataformas IoT facilitan la recopilación, almacenamiento, visualización y análisis de datos de sensores.

## ¿Qué es una plataforma IoT?

Una plataforma IoT proporciona:
- **Ingesta de datos**: recepción de datos desde dispositivos
- **Almacenamiento**: bases de datos para series temporales
- **Visualización**: dashboards y gráficos
- **Análisis**: procesamiento y alertas
- **Control**: envío de comandos a dispositivos
- **APIs**: integración con otras aplicaciones

## Arquitectura típica IoT

```
ESP32-C6 → Wi-Fi/BLE → Gateway/Cloud → Dashboard
  ↓                                        ↓
Sensores                               Visualización
                                          Análisis
                                          Alertas
```

## Plataformas populares

### 1. ThingSpeak

**Características**:
- Gratuito (hasta 3 millones de mensajes/año)
- MATLAB integrado para análisis
- Visualización de gráficos
- API REST simple

**Ventajas**: Fácil de usar, gratuito
**Desventajas**: Límite de velocidad (15 seg entre mensajes)

#### Ejemplo con ESP32-C6

```c
#include "esp_http_client.h"

#define THINGSPEAK_API_KEY "TU_API_KEY"
#define THINGSPEAK_URL "http://api.thingspeak.com/update"

void thingspeak_send(float temperature, float humidity)
{
    char url[256];
    snprintf(url, sizeof(url), 
            "%s?api_key=%s&field1=%.2f&field2=%.2f",
            THINGSPEAK_URL, THINGSPEAK_API_KEY, 
            temperature, humidity);
    
    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
    };
    
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);
    
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Datos enviados a ThingSpeak");
    }
    
    esp_http_client_cleanup(client);
}
```

### 2. Adafruit IO

**Características**:
- Gratuito (30 datos/minuto)
- Dashboards personalizables
- MQTT y REST API
- Control bidireccional

**Ventajas**: Interface amigable, widgets variados
**Desventajas**: Límites en plan gratuito

#### Ejemplo MQTT

```c
#include "mqtt_client.h"

#define AIO_SERVER   "io.adafruit.com"
#define AIO_PORT     1883
#define AIO_USERNAME "tu_usuario"
#define AIO_KEY      "tu_aio_key"
#define AIO_FEED     "temperatura"

esp_mqtt_client_handle_t mqtt_client;

void adafruit_io_init(void)
{
    char uri[128];
    snprintf(uri, sizeof(uri), "mqtt://%s:%d", AIO_SERVER, AIO_PORT);
    
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri,
        .credentials.username = AIO_USERNAME,
        .credentials.authentication.password = AIO_KEY,
    };
    
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(mqtt_client);
}

void adafruit_io_publish(float value)
{
    char topic[128];
    char data[32];
    
    snprintf(topic, sizeof(topic), "%s/feeds/%s", AIO_USERNAME, AIO_FEED);
    snprintf(data, sizeof(data), "%.2f", value);
    
    esp_mqtt_client_publish(mqtt_client, topic, data, 0, 1, 0);
}
```

### 3. Blynk

**Características**:
- App móvil iOS/Android
- Control en tiempo real
- Widgets interactivos
- Notificaciones push

**Ventajas**: App móvil potente, fácil configuración
**Desventajas**: Requiere app propietaria

#### Ejemplo

```c
#include "esp_http_client.h"

#define BLYNK_TOKEN "TU_BLYNK_TOKEN"
#define BLYNK_SERVER "blynk.cloud"

void blynk_virtual_write(int pin, float value)
{
    char url[256];
    snprintf(url, sizeof(url), 
            "http://%s/external/api/update?token=%s&V%d=%.2f",
            BLYNK_SERVER, BLYNK_TOKEN, pin, value);
    
    esp_http_client_config_t config = {.url = url};
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}

float blynk_virtual_read(int pin)
{
    char url[256];
    snprintf(url, sizeof(url), 
            "http://%s/external/api/get?token=%s&V%d",
            BLYNK_SERVER, BLYNK_TOKEN, pin);
    
    // Implementar lectura HTTP y parseo
    return 0.0;
}
```

### 4. MQTT Brokers públicos

#### HiveMQ (público)

```c
esp_mqtt_client_config_t mqtt_cfg = {
    .broker.address.uri = "mqtt://broker.hivemq.com:1883",
};
```

#### Mosquitto (local)

Instalar en Raspberry Pi o servidor local:
```bash
sudo apt install mosquitto mosquitto-clients
sudo systemctl start mosquitto
```

Código ESP32-C6:
```c
esp_mqtt_client_config_t mqtt_cfg = {
    .broker.address.uri = "mqtt://192.168.1.100:1883",
};
```

### 5. AWS IoT Core

**Características**:
- Escalable empresarial
- Certificados X.509 para seguridad
- Integración con servicios AWS
- Reglas y acciones automatizadas

**Ventajas**: Robusto, seguro, escalable
**Desventajas**: Complejo, costoso para grandes volúmenes

#### Configuración (simplificada)

```c
#include "mqtt_client.h"

#define AWS_IOT_ENDPOINT "xxxxx.iot.us-east-1.amazonaws.com"
#define AWS_IOT_TOPIC    "$aws/things/mi-dispositivo/shadow/update"

extern const uint8_t client_cert_pem_start[] asm("_binary_client_crt_start");
extern const uint8_t client_key_pem_start[] asm("_binary_client_key_start");
extern const uint8_t server_cert_pem_start[] asm("_binary_aws_root_ca_start");

void aws_iot_init(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtts://" AWS_IOT_ENDPOINT ":8883",
        .broker.verification.certificate = (const char *)server_cert_pem_start,
        .credentials.authentication.certificate = (const char *)client_cert_pem_start,
        .credentials.authentication.key = (const char *)client_key_pem_start,
    };
    
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);
}
```

### 6. Firebase Realtime Database

**Características**:
- Base de datos NoSQL en tiempo real
- Sincronización automática
- Gratuito hasta cierto límite
- SDK para web y móvil

#### Ejemplo REST API

```c
#include "esp_http_client.h"
#include "cJSON.h"

#define FIREBASE_HOST "tu-proyecto.firebaseio.com"
#define FIREBASE_AUTH "tu_database_secret"

void firebase_send_data(float temperature)
{
    // Crear JSON
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "temperature", temperature);
    cJSON_AddNumberToObject(root, "timestamp", time(NULL));
    char *json_str = cJSON_Print(root);
    
    // URL
    char url[256];
    snprintf(url, sizeof(url), 
            "https://%s/sensors/temp.json?auth=%s",
            FIREBASE_HOST, FIREBASE_AUTH);
    
    // HTTP PUT
    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_PUT,
        .cert_pem = NULL,  // Agregar certificado raíz
    };
    
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, json_str, strlen(json_str));
    
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
    
    cJSON_Delete(root);
    free(json_str);
}
```

## Formatos de datos

### JSON

El formato más común para IoT:

```c
#include "cJSON.h"

char* create_sensor_json(float temp, float hum, float pres)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "device_id", "ESP32-C6-001");
    cJSON_AddNumberToObject(root, "timestamp", time(NULL));
    
    cJSON *data = cJSON_CreateObject();
    cJSON_AddNumberToObject(data, "temperature", temp);
    cJSON_AddNumberToObject(data, "humidity", hum);
    cJSON_AddNumberToObject(data, "pressure", pres);
    cJSON_AddItemToObject(root, "data", data);
    
    char *json_string = cJSON_Print(root);
    cJSON_Delete(root);
    
    return json_string;  // Recordar hacer free()
}
```

Ejemplo de JSON generado:
```json
{
  "device_id": "ESP32-C6-001",
  "timestamp": 1672531200,
  "data": {
    "temperature": 25.5,
    "humidity": 65.2,
    "pressure": 1013.25
  }
}
```

### ProtoBuf (Protocol Buffers)

Formato binario eficiente de Google:

**Ventajas**: Menor tamaño, más rápido
**Desventajas**: No legible por humanos

### MessagePack

Formato binario similar a JSON pero más compacto.

## Seguridad

### 1. Autenticación

**API Keys**: incluir en header o query string
```c
esp_http_client_set_header(client, "X-API-Key", "tu_api_key");
```

**Tokens JWT**: JSON Web Tokens para autenticación
```c
esp_http_client_set_header(client, "Authorization", "Bearer eyJ0eXAi...");
```

### 2. Encriptación

**TLS/SSL**: siempre usar HTTPS/MQTTS en producción
```c
esp_http_client_config_t config = {
    .url = "https://api.segura.com",
    .cert_pem = server_root_cert_pem_start,
};
```

### 3. Certificados

Para AWS IoT, Azure IoT, etc.:
```c
extern const uint8_t client_cert_pem_start[] asm("_binary_cert_pem_start");
extern const uint8_t client_key_pem_start[] asm("_binary_key_pem_start");
```

## Almacenamiento local

### NVS (Non-Volatile Storage)

Guardar datos localmente antes de enviar:

```c
#include "nvs_flash.h"
#include "nvs.h"

void save_sensor_data(float value)
{
    nvs_handle_t handle;
    nvs_open("storage", NVS_READWRITE, &handle);
    
    uint32_t value_int = (uint32_t)(value * 100);  // Guardar como entero
    nvs_set_u32(handle, "last_temp", value_int);
    nvs_commit(handle);
    nvs_close(handle);
}
```

### SD Card

Para grandes volúmenes de datos:

```c
#include <stdio.h>

void log_to_sd(float temp, float hum)
{
    FILE *f = fopen("/sd/data.csv", "a");
    if (f) {
        fprintf(f, "%ld,%.2f,%.2f\n", time(NULL), temp, hum);
        fclose(f);
    }
}
```

## Buenas prácticas

1. **Buffering**: acumular datos antes de enviar para reducir consumo
2. **Retry logic**: reintentar en caso de fallo de conexión
3. **Timestamps**: incluir siempre marca de tiempo
4. **Baterías**: considerar consumo de energía (deep sleep)
5. **Seguridad**: usar TLS/SSL en producción
6. **Rate limiting**: respetar límites de la plataforma
7. **Offline mode**: guardar datos localmente si no hay conexión

## Ejemplo completo: Sistema de monitoreo

```c
#include "esp_wifi.h"
#include "mqtt_client.h"
#include "cJSON.h"
#include "driver/adc.h"

#define MQTT_BROKER "mqtt://broker.hivemq.com:1883"
#define MQTT_TOPIC  "esp32c6/sensores"

esp_mqtt_client_handle_t mqtt_client;

void sensor_task(void *pvParameters)
{
    while (1) {
        // Leer sensores
        float temperature = read_temperature();
        float humidity = read_humidity();
        
        // Crear JSON
        cJSON *root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "device", "sensor-001");
        cJSON_AddNumberToObject(root, "temp", temperature);
        cJSON_AddNumberToObject(root, "hum", humidity);
        char *json = cJSON_PrintUnformatted(root);
        
        // Publicar
        esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC, json, 0, 1, 0);
        
        cJSON_Delete(root);
        free(json);
        
        vTaskDelay(pdMS_TO_TICKS(60000));  // Cada minuto
    }
}

void app_main(void)
{
    wifi_init_sta();
    mqtt_init();
    xTaskCreate(sensor_task, "sensor_task", 4096, NULL, 5, NULL);
}
```

## Comparación de plataformas

| Plataforma | Protocolo | Gratuito | Dificultad | Mejor para |
|------------|-----------|----------|------------|------------|
| ThingSpeak | HTTP/MQTT | Sí (limitado) | Fácil | Prototipado |
| Adafruit IO | HTTP/MQTT | Sí (limitado) | Fácil | Hobbyistas |
| Blynk | HTTP/TCP | Sí (limitado) | Fácil | Apps móviles |
| HiveMQ | MQTT | Sí (público) | Media | Testing |
| Firebase | HTTP/REST | Sí (limitado) | Media | Web/Móvil |
| AWS IoT | MQTTS | No | Alta | Empresarial |

## Próximos pasos

- [Lab 4: Publicación Wi-Fi](../examples/lab04-wifi.md)
- [Lab 5: Bluetooth LE](../examples/lab05-ble.md)
- [Wi-Fi y Bluetooth LE](./connectivity.md)

## Referencias

- [ThingSpeak Documentation](https://www.mathworks.com/help/thingspeak/)
- [Adafruit IO](https://io.adafruit.com/)
- [MQTT.org](https://mqtt.org/)
- [AWS IoT Core](https://docs.aws.amazon.com/iot/)
- [Firebase Realtime Database](https://firebase.google.com/docs/database)
