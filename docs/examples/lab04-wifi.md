# Lab 4: Publicación de Datos mediante Wi-Fi

En esta práctica aprenderás a conectar el ESP32-C6 a Wi-Fi y publicar datos de sensores a plataformas IoT.

## Objetivos

- Conectar ESP32-C6 a red Wi-Fi
- Enviar datos HTTP a servidor web
- Publicar datos vía MQTT
- Integrar con ThingSpeak
- Crear dashboard web simple

## Materiales

- Placa Pulsar C6 (ESP32-C6)
- Sensor BME280 o similar (opcional)
- Red Wi-Fi disponible
- Cuenta en ThingSpeak (gratuita)

## Práctica 1: Conexión Wi-Fi básica

### Código: Conectar a Wi-Fi

```c
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#define WIFI_SSID      "TU_RED_WIFI"
#define WIFI_PASS      "TU_CONTRASEÑA"

static const char *TAG = "WIFI";
static EventGroupHandle_t wifi_event_group;
const int WIFI_CONNECTED_BIT = BIT0;

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
        ESP_LOGI(TAG, "Conectando a Wi-Fi...");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Desconectado. Reintentando...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "IP obtenida: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(void)
{
    wifi_event_group = xEventGroupCreate();
    
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));
    
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
    
    ESP_LOGI(TAG, "Wi-Fi Station inicializado");
    
    // Esperar conexión
    xEventGroupWaitBits(wifi_event_group,
                       WIFI_CONNECTED_BIT,
                       pdFALSE,
                       pdFALSE,
                       portMAX_DELAY);
    
    ESP_LOGI(TAG, "Conectado a Wi-Fi!");
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init_sta();
    
    // Tu código aquí
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

## Práctica 2: HTTP GET Request

### Código: Consultar API

```c
#include "esp_http_client.h"
#include "esp_log.h"

static const char *TAG = "HTTP";

esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            if (!esp_http_client_is_chunked_response(evt->client)) {
                printf("%.*s", evt->data_len, (char *)evt->data);
            }
            break;
        default:
            break;
    }
    return ESP_OK;
}

void http_get_example(void)
{
    esp_http_client_config_t config = {
        .url = "http://httpbin.org/get",
        .event_handler = http_event_handler,
    };
    
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);
    
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET Status = %d, Content-Length = %lld",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    
    esp_http_client_cleanup(client);
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init_sta();
    
    // Esperar conexión Wi-Fi
    vTaskDelay(pdMS_TO_TICKS(5000));
    
    http_get_example();
}
```

## Práctica 3: Enviar datos a ThingSpeak

### Paso 1: Configurar ThingSpeak

1. Crear cuenta en [ThingSpeak](https://thingspeak.com/)
2. Crear nuevo canal con campos:
   - Field 1: Temperature
   - Field 2: Humidity
   - Field 3: Pressure
3. Copiar **Write API Key**

### Código: Publicar a ThingSpeak

```c
#include "esp_http_client.h"
#include "esp_log.h"
#include <string.h>

#define THINGSPEAK_API_KEY "TU_API_KEY_AQUI"
#define THINGSPEAK_URL "http://api.thingspeak.com/update"

static const char *TAG = "THINGSPEAK";

void thingspeak_send_data(float temperature, float humidity, float pressure)
{
    char url[256];
    snprintf(url, sizeof(url), 
            "%s?api_key=%s&field1=%.2f&field2=%.2f&field3=%.2f",
            THINGSPEAK_URL, THINGSPEAK_API_KEY, 
            temperature, humidity, pressure);
    
    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
    };
    
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);
    
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Datos enviados. HTTP Status = %d",
                esp_http_client_get_status_code(client));
    } else {
        ESP_LOGE(TAG, "Error enviando datos: %s", esp_err_to_name(err));
    }
    
    esp_http_client_cleanup(client);
}

void sensor_publish_task(void *pvParameters)
{
    while (1) {
        // Leer sensores (ejemplo con valores dummy)
        float temperature = 25.5;
        float humidity = 65.2;
        float pressure = 1013.25;
        
        thingspeak_send_data(temperature, humidity, pressure);
        
        // ThingSpeak: mínimo 15 segundos entre actualizaciones (plan gratuito)
        vTaskDelay(pdMS_TO_TICKS(20000));
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init_sta();
    
    xTaskCreate(sensor_publish_task, "publish", 4096, NULL, 5, NULL);
}
```

## Práctica 4: MQTT con HiveMQ

### Código: MQTT Publish/Subscribe

```c
#include "mqtt_client.h"
#include "esp_log.h"
#include "cJSON.h"

static const char *TAG = "MQTT";

#define MQTT_BROKER "mqtt://broker.hivemq.com:1883"
#define MQTT_TOPIC_PUB "esp32c6/sensors/data"
#define MQTT_TOPIC_SUB "esp32c6/control/led"

esp_mqtt_client_handle_t mqtt_client;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT conectado");
            esp_mqtt_client_subscribe(mqtt_client, MQTT_TOPIC_SUB, 0);
            ESP_LOGI(TAG, "Suscrito a: %s", MQTT_TOPIC_SUB);
            break;
            
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT desconectado");
            break;
            
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "Mensaje recibido: %.*s", event->data_len, event->data);
            ESP_LOGI(TAG, "Desde topic: %.*s", event->topic_len, event->topic);
            break;
            
        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "Error MQTT");
            break;
            
        default:
            break;
    }
}

void mqtt_init(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER,
    };
    
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, 
                                   mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

void mqtt_publish_sensor_data(float temperature, float humidity)
{
    // Crear JSON
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "device_id", "pulsar-c6-001");
    cJSON_AddNumberToObject(root, "timestamp", time(NULL));
    cJSON_AddNumberToObject(root, "temperature", temperature);
    cJSON_AddNumberToObject(root, "humidity", humidity);
    
    char *json_string = cJSON_PrintUnformatted(root);
    
    int msg_id = esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC_PUB, 
                                         json_string, 0, 1, 0);
    
    ESP_LOGI(TAG, "Mensaje publicado, ID: %d", msg_id);
    
    cJSON_Delete(root);
    free(json_string);
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init_sta();
    
    mqtt_init();
    
    // Esperar conexión MQTT
    vTaskDelay(pdMS_TO_TICKS(5000));
    
    while (1) {
        mqtt_publish_sensor_data(25.5, 65.2);
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
```

### Prueba con MQTT Explorer

Descarga [MQTT Explorer](http://mqtt-explorer.com/) para visualizar mensajes:
1. Conectar a `broker.hivemq.com`
2. Suscribirse a `esp32c6/sensors/#`
3. Ver mensajes en tiempo real

## Práctica 5: Servidor HTTP simple

### Código: Web Server

```c
#include "esp_http_server.h"
#include "esp_log.h"

static const char *TAG = "HTTP_SERVER";

float current_temperature = 25.5;
float current_humidity = 65.2;

static esp_err_t root_handler(httpd_req_t *req)
{
    const char *html = 
        "<!DOCTYPE html><html><head>"
        "<meta charset='utf-8'>"
        "<meta name='viewport' content='width=device-width, initial-scale=1'>"
        "<title>ESP32-C6 Sensor</title>"
        "<style>body{font-family:Arial;text-align:center;padding:50px;}"
        "h1{color:#0066cc;}</style>"
        "</head><body>"
        "<h1>ESP32-C6 Sensor Monitor</h1>"
        "<p>Temperatura: %.2f °C</p>"
        "<p>Humedad: %.2f %%</p>"
        "</body></html>";
    
    char response[512];
    snprintf(response, sizeof(response), html, current_temperature, current_humidity);
    
    httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t api_handler(httpd_req_t *req)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "temperature", current_temperature);
    cJSON_AddNumberToObject(root, "humidity", current_humidity);
    cJSON_AddNumberToObject(root, "timestamp", time(NULL));
    
    char *json_string = cJSON_Print(root);
    
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_string, strlen(json_string));
    
    cJSON_Delete(root);
    free(json_string);
    
    return ESP_OK;
}

httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t root_uri = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = root_handler,
        };
        httpd_register_uri_handler(server, &root_uri);
        
        httpd_uri_t api_uri = {
            .uri = "/api/data",
            .method = HTTP_GET,
            .handler = api_handler,
        };
        httpd_register_uri_handler(server, &api_uri);
        
        ESP_LOGI(TAG, "Servidor web iniciado");
    }
    
    return server;
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init_sta();
    
    start_webserver();
    
    ESP_LOGI(TAG, "Accede a http://<IP_ESP32>/");
}
```

## Ejercicios

### Ejercicio 1: Dashboard completo
Integra sensores BME280 + LDR y publica todos los datos a ThingSpeak.

### Ejercicio 2: Control remoto
Usa MQTT para controlar un LED desde un smartphone con app MQTT (ej: MQTT Dash).

### Ejercicio 3: Data logging
Guarda datos localmente en SD y sincroniza con servidor cuando hay Wi-Fi.

### Ejercicio 4: OTA Updates
Implementa actualización de firmware over-the-air mediante Wi-Fi.

## Solución de problemas

| Problema | Solución |
|----------|----------|
| No conecta a Wi-Fi | Verificar SSID/password, tipo de seguridad |
| HTTP timeout | Verificar conectividad, URL correcta |
| MQTT desconexiones | Usar QoS 1, implementar reconnect logic |
| Servidor web lento | Reducir complejidad HTML, usar async handlers |

## Seguridad

::: warning Producción
Para producción, siempre usa:
- HTTPS (no HTTP)
- MQTTS (MQTT over TLS)
- Certificados SSL/TLS
- Tokens de autenticación
:::

## Próximos pasos

- [Lab 5: Bluetooth LE](./lab05-ble.md)
- [Guía Wi-Fi y Bluetooth](../guide/connectivity.md)
- [Plataformas IoT](../guide/iot-platforms.md)

## Referencias

- [ESP-IDF Wi-Fi](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/network/esp_wifi.html)
- [ESP-IDF HTTP Client](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/protocols/esp_http_client.html)
- [ESP-MQTT](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/protocols/mqtt.html)
- [ThingSpeak API](https://www.mathworks.com/help/thingspeak/rest-api.html)
