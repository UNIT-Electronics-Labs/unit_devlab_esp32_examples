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
- DHT11 sensor de temperatura y humedad KY-015
- TMP235 sensor de temperatura analógico (alternativa)
- Red Wi-Fi disponible
- Cuenta en ThingSpeak (gratuita)

## Código base Arduino

Este sketch conecta la Pulsar C6 a Wi-Fi y publica temperatura/humedad del DHT11
por HTTP. Instala `DHT sensor library` desde **Tools > Manage Libraries**.

```cpp
#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

const char* WIFI_SSID = "TU_RED_WIFI";
const char* WIFI_PASS = "TU_CONTRASENA";
const char* THINGSPEAK_API_KEY = "TU_API_KEY_AQUI";

const int DHT_PIN = 4;
DHT dht(DHT_PIN, DHT11);

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Conectando a Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Error leyendo DHT11");
      delay(2000);
      return;
    }

    String url = "http://api.thingspeak.com/update?api_key=";
    url += THINGSPEAK_API_KEY;
    url += "&field1=";
    url += String(temperature, 2);
    url += "&field2=";
    url += String(humidity, 2);

    HTTPClient http;
    http.begin(url);
    int status = http.GET();

    Serial.print("HTTP status: ");
    Serial.println(status);
    http.end();
  }

  delay(20000);
}
```

### Conexiones DHT11 KY-015

```
DHT11 KY-015
━━━━━━━━━━━━━━━━━━━━
S / DATA  ───── GPIO4
VCC       ───── 3.3V
GND       ───── GND
```

## Referencia avanzada ESP-IDF

Si necesitas portar esta practica a ESP-IDF, usa `esp_wifi`, `esp_http_client` y publica los mismos campos de DHT11 o TMP235. Para el curso base, usa el sketch Arduino anterior.

## Ejercicios

### Ejercicio 1: Dashboard completo
Integra DHT11 + TMP235 y publica temperatura, humedad y temperatura analógica a ThingSpeak.

### Ejercicio 2: Control remoto
Usa MQTT para publicar el estado del TTP223B desde la Pulsar C6.

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

- [Arduino WiFi library](https://docs.arduino.cc/libraries/wifi/)
- [PubSubClient](https://pubsubclient.knolleary.net/)
- [ThingSpeak API](https://www.mathworks.com/help/thingspeak/rest-api.html)
- [ESP-IDF Wi-Fi](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/network/esp_wifi.html) - referencia avanzada
- [ESP-IDF HTTP Client](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/protocols/esp_http_client.html) - referencia avanzada
- [ESP-MQTT](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/protocols/mqtt.html) - referencia avanzada
