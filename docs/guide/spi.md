# Comunicación SPI

SPI (Serial Peripheral Interface) es un protocolo de comunicación serial síncrono de alta velocidad para periféricos.

## Características de SPI

| Característica | Descripción |
|----------------|-------------|
| **Pines** | 4: MOSI, MISO, SCK, CS |
| **Topología** | Maestro-esclavo (un maestro, múltiples esclavos) |
| **Velocidad** | Hasta 80 MHz en ESP32-C6 |
| **Modo** | Full-duplex (envío y recepción simultánea) |
| **Distancia** | < 3 metros típico |

## Ventajas e inconvenientes

**Ventajas**:
- **Muy rápido**: 10-100x más rápido que I²C
- **Full-duplex**: puede enviar y recibir simultáneamente
- **Simple**: no requiere pull-ups
- **Sin direcciones**: selección por CS

**Inconvenientes**:
- **Más pines**: 4 líneas (más CS por dispositivo)
- **Un maestro**: no es multi-maestro
- **Sin ACK**: no hay confirmación de recepción

## Señales SPI

```
Maestro          Esclavo 1
MOSI ────────────→ MOSI (Master Out Slave In)
MISO ←────────────  MISO (Master In Slave Out)
SCK  ────────────→ SCK (Clock)
CS1  ────────────→ CS (Chip Select)
```

### MOSI (Master Out Slave In)
Datos del maestro → esclavo

### MISO (Master In Slave Out)
Datos del esclavo → maestro

### SCK (Serial Clock)
Señal de reloj generada por el maestro

### CS (Chip Select)
Selecciona el esclavo activo (activo en LOW)

## Múltiples dispositivos SPI

```
ESP32-C6
MOSI ──┬─── Dispositivo 1
       └─── Dispositivo 2

MISO ──┬─── Dispositivo 1
       └─── Dispositivo 2

SCK  ──┬─── Dispositivo 1
       └─── Dispositivo 2

CS1  ────── Dispositivo 1
CS2  ────── Dispositivo 2
```

Cada dispositivo necesita su propio pin CS.

## Modos SPI

SPI tiene 4 modos definidos por **CPOL** (polaridad del reloj) y **CPHA** (fase del reloj):

| Modo | CPOL | CPHA | Reloj inactivo | Muestreo |
|------|------|------|----------------|----------|
| 0 | 0 | 0 | LOW | Flanco ascendente |
| 1 | 0 | 1 | LOW | Flanco descendente |
| 2 | 1 | 0 | HIGH | Flanco descendente |
| 3 | 1 | 1 | HIGH | Flanco ascendente |

**Modo 0** es el más común.

## Pines SPI en ESP32-C6

ESP32-C6 tiene 2 controladores SPI disponibles: **SPI2** y **SPI3** (SPI0/SPI1 reservados para flash).

### Pines por defecto (configurables)

| Señal | GPIO sugerido |
|-------|---------------|
| MOSI | GPIO19 |
| MISO | GPIO18 |
| SCK | GPIO20 |
| CS | GPIO21 (o cualquier GPIO) |

::: tip Pines flexibles
Los pines SPI en ESP32-C6 son totalmente configurables gracias a la matriz GPIO.
:::

## API SPI en ESP-IDF

### Inicialización del bus SPI

```c
#include "driver/spi_master.h"
#include "driver/gpio.h"

#define PIN_NUM_MISO  18
#define PIN_NUM_MOSI  19
#define PIN_NUM_CLK   20
#define PIN_NUM_CS    21

spi_bus_config_t buscfg = {
    .miso_io_num = PIN_NUM_MISO,
    .mosi_io_num = PIN_NUM_MOSI,
    .sclk_io_num = PIN_NUM_CLK,
    .quadwp_io_num = -1,  // No usado
    .quadhd_io_num = -1,  // No usado
    .max_transfer_sz = 4096,
};

// Inicializar bus SPI2
ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
```

### Agregar dispositivo

```c
spi_device_handle_t spi_device;

spi_device_interface_config_t devcfg = {
    .clock_speed_hz = 10 * 1000 * 1000,  // 10 MHz
    .mode = 0,                           // SPI modo 0
    .spics_io_num = PIN_NUM_CS,
    .queue_size = 7,
    .flags = 0,
    .pre_cb = NULL,
};

// Agregar dispositivo al bus
ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &spi_device));
```

### Enviar datos

```c
void spi_write_byte(spi_device_handle_t spi, uint8_t data)
{
    spi_transaction_t trans = {
        .length = 8,              // 8 bits
        .tx_buffer = &data,
        .rx_buffer = NULL,
    };
    
    ESP_ERROR_CHECK(spi_device_transmit(spi, &trans));
}
```

### Leer datos

```c
uint8_t spi_read_byte(spi_device_handle_t spi)
{
    uint8_t rx_data;
    
    spi_transaction_t trans = {
        .length = 8,
        .tx_buffer = NULL,
        .rx_buffer = &rx_data,
    };
    
    ESP_ERROR_CHECK(spi_device_transmit(spi, &trans));
    return rx_data;
}
```

### Transmisión full-duplex

```c
void spi_write_read(spi_device_handle_t spi, uint8_t *tx_data, 
                   uint8_t *rx_data, size_t len)
{
    spi_transaction_t trans = {
        .length = len * 8,        // En bits
        .tx_buffer = tx_data,
        .rx_buffer = rx_data,
    };
    
    ESP_ERROR_CHECK(spi_device_transmit(spi, &trans));
}
```

## Ejemplo: Sensor de temperatura MAX6675 (termopar)

```c
#include "driver/spi_master.h"
#include "esp_log.h"

#define PIN_MISO  18
#define PIN_MOSI  19  // No usado por MAX6675
#define PIN_CLK   20
#define PIN_CS    21

static const char *TAG = "MAX6675";

spi_device_handle_t max6675_init(void)
{
    // Configurar bus SPI
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_MISO,
        .mosi_io_num = PIN_MOSI,
        .sclk_io_num = PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    
    // Configurar dispositivo MAX6675
    spi_device_handle_t spi;
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1 * 1000 * 1000,  // 1 MHz (max 4.3 MHz)
        .mode = 0,
        .spics_io_num = PIN_CS,
        .queue_size = 1,
        .flags = SPI_DEVICE_HALFDUPLEX,
    };
    spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
    
    return spi;
}

float max6675_read_temperature(spi_device_handle_t spi)
{
    uint8_t rx_data[2];
    
    spi_transaction_t trans = {
        .length = 16,              // 16 bits
        .rx_buffer = rx_data,
        .tx_buffer = NULL,
    };
    
    ESP_ERROR_CHECK(spi_device_transmit(spi, &trans));
    
    // Combinar bytes
    uint16_t raw = (rx_data[0] << 8) | rx_data[1];
    
    // Verificar bit de error (bit 2)
    if (raw & 0x04) {
        ESP_LOGE(TAG, "Termopar desconectado");
        return -1;
    }
    
    // Extraer temperatura (bits 15-3, resolución 0.25°C)
    raw >>= 3;
    float temperature = raw * 0.25;
    
    return temperature;
}

void app_main(void)
{
    spi_device_handle_t max6675 = max6675_init();
    
    while (1) {
        float temp = max6675_read_temperature(max6675);
        if (temp >= 0) {
            ESP_LOGI(TAG, "Temperatura: %.2f °C", temp);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

## Ejemplo: Pantalla LCD ST7735 (TFT)

```c
#include "driver/spi_master.h"
#include "driver/gpio.h"

#define PIN_MISO  18
#define PIN_MOSI  19
#define PIN_CLK   20
#define PIN_CS    21
#define PIN_DC    22  // Data/Command
#define PIN_RST   23  // Reset

spi_device_handle_t lcd_init(void)
{
    // Bus SPI
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_MISO,
        .mosi_io_num = PIN_MOSI,
        .sclk_io_num = PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 320 * 240 * 2,
    };
    spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    
    // Dispositivo
    spi_device_handle_t spi;
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 26 * 1000 * 1000,  // 26 MHz
        .mode = 0,
        .spics_io_num = PIN_CS,
        .queue_size = 7,
        .flags = 0,
    };
    spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
    
    // Configurar pines DC y RST
    gpio_set_direction(PIN_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_RST, GPIO_MODE_OUTPUT);
    
    // Reset de LCD
    gpio_set_level(PIN_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(PIN_RST, 1);
    
    return spi;
}

void lcd_cmd(spi_device_handle_t spi, uint8_t cmd)
{
    gpio_set_level(PIN_DC, 0);  // Modo comando
    spi_write_byte(spi, cmd);
}

void lcd_data(spi_device_handle_t spi, uint8_t data)
{
    gpio_set_level(PIN_DC, 1);  // Modo datos
    spi_write_byte(spi, data);
}

void lcd_fill_color(spi_device_handle_t spi, uint16_t color)
{
    // Llenar toda la pantalla con un color
    lcd_cmd(spi, 0x2C);  // Comando RAMWR
    
    gpio_set_level(PIN_DC, 1);
    
    uint16_t pixels = 320 * 240;
    for (int i = 0; i < pixels; i++) {
        spi_write_byte(spi, color >> 8);
        spi_write_byte(spi, color & 0xFF);
    }
}
```

## Tarjetas SD con SPI

```c
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/spi_common.h"

#define PIN_MISO  18
#define PIN_MOSI  19
#define PIN_CLK   20
#define PIN_CS    21

void sd_card_init(void)
{
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
    };
    
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = PIN_MISO,
        .sclk_io_num = PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    
    spi_bus_initialize(host.slot, &bus_cfg, SPI_DMA_CH_AUTO);
    
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_CS;
    slot_config.host_id = host.slot;
    
    sdmmc_card_t *card;
    esp_vfs_fat_sdspi_mount("/sd", &host, &slot_config, &mount_config, &card);
    
    ESP_LOGI(TAG, "Tarjeta SD montada en /sd");
}
```

## SPI vs I²C

| Característica | I²C | SPI |
|----------------|-----|-----|
| **Velocidad** | 100 kHz - 1 MHz | Hasta 80 MHz |
| **Pines** | 2 (SDA, SCL) | 4+ (MOSI, MISO, SCK, CS...) |
| **Topología** | Multi-maestro | Un maestro |
| **Distancia** | < 1 metro | < 3 metros |
| **Uso típico** | Sensores | Pantallas, SD, RF |

## Solución de problemas

### No hay respuesta del dispositivo

1. Verificar conexiones MOSI, MISO, SCK, CS
2. Verificar modo SPI (CPOL, CPHA)
3. Reducir velocidad de reloj
4. Verificar alimentación 3.3V

### Datos corruptos

1. Cables demasiado largos → usar <20cm
2. Velocidad demasiado alta → reducir `clock_speed_hz`
3. Interferencia → usar cables blindados o twisted-pair

### CS no funciona

Verificar que CS está configurado correctamente y es activo en LOW.

## Próximos pasos

- [Comunicación I²C](./i2c.md)
- [Sensores y transductores](./sensors.md)
- Próximos labs usarán SPI para pantallas y almacenamiento

## Referencias

- [ESP32-C6 SPI Master](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/peripherals/spi_master.html)
- [SPI Protocol](https://en.wikipedia.org/wiki/Serial_Peripheral_Interface)
- [Understanding SPI](https://learn.sparkfun.com/tutorials/serial-peripheral-interface-spi)
