# Comunicación I²C

I²C (Inter-Integrated Circuit) es un bus de comunicación serial para conectar sensores y periféricos de corto alcance.

## Características de I²C

| Característica | Descripción |
|----------------|-------------|
| **Pines** | 2: SDA (datos), SCL (reloj) |
| **Topología** | Multi-maestro, multi-esclavo |
| **Velocidad** | Estándar: 100 kHz, Rápido: 400 kHz, Rápido+: 1 MHz |
| **Direccionamiento** | 7 bits (128 dispositivos) o 10 bits |
| **Distancia** | <1 metro (típico) |
| **Alimentación** | Requiere pull-up resistors |

## Ventajas e inconvenientes

**Ventajas**:
- Solo 2 cables (más GND y VCC)
- Múltiples dispositivos en el mismo bus
- Amplia disponibilidad de sensores I²C
- Protocolo bien establecido

**Inconvenientes**:
- Más lento que SPI
- Requiere resistencias pull-up
- Distancias cortas
- Posibles conflictos de dirección

## Hardware

### Conexión básica

```
ESP32-C6          Sensor
GPIO6 (SDA) ──────── SDA
GPIO7 (SCL) ──────── SCL
3.3V ───────────────── VCC
GND ────────────────── GND

        4.7kΩ    4.7kΩ
         │        │
         ├────────┤
         │        │
       3.3V      3.3V
```

::: tip Resistencias Pull-up
Son **obligatorias** en SDA y SCL. Valores típicos: 2.2kΩ - 10kΩ.
- Bus corto (<10cm), pocos dispositivos: 4.7kΩ
- Bus largo o muchos dispositivos: 2.2kΩ
- Algunos módulos ya incluyen pull-ups integrados
:::

### Múltiples dispositivos

```
ESP32-C6
  SDA ──┬─── Sensor 1 (0x76)
        ├─── Sensor 2 (0x48)
        └─── Display (0x3C)

  SCL ──┬─── Sensor 1
        ├─── Sensor 2
        └─── Display
```

Cada dispositivo necesita una dirección única.

## Direccionamiento I²C

### Encontrar dirección de un dispositivo

```c
#include "driver/i2c.h"
#include "esp_log.h"

#define I2C_MASTER_SCL_IO    7
#define I2C_MASTER_SDA_IO    6
#define I2C_MASTER_NUM       I2C_NUM_0
#define I2C_MASTER_FREQ_HZ   100000

static const char *TAG = "I2C_SCANNER";

void i2c_master_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    
    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));
}

void i2c_scanner(void)
{
    ESP_LOGI(TAG, "Escaneando bus I2C...");
    
    for (uint8_t addr = 1; addr < 127; addr++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);
        
        esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(50));
        i2c_cmd_link_delete(cmd);
        
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "Dispositivo encontrado en 0x%02X", addr);
        }
    }
    
    ESP_LOGI(TAG, "Escaneo completado");
}

void app_main(void)
{
    i2c_master_init();
    i2c_scanner();
}
```

### Direcciones comunes

| Sensor | Dirección por defecto |
|--------|-----------------------|
| BME280 | 0x76 o 0x77 |
| BMP180 | 0x77 |
| MPU6050 | 0x68 o 0x69 |
| OLED SSD1306 | 0x3C o 0x3D |
| ADS1115 (ADC) | 0x48 - 0x4B |
| MCP4725 (DAC) | 0x60 - 0x63 |
| BH1750 (luz) | 0x23 o 0x5C |
| DS3231 (RTC) | 0x68 |

## API de I²C en ESP-IDF

### Inicialización

```c
#include "driver/i2c.h"

#define I2C_MASTER_SCL_IO    7
#define I2C_MASTER_SDA_IO    6
#define I2C_MASTER_NUM       I2C_NUM_0
#define I2C_MASTER_FREQ_HZ   400000  // 400 kHz

void i2c_master_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
        .clk_flags = 0,
    };
    
    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));
}
```

### Escritura simple

```c
esp_err_t i2c_write_byte(uint8_t dev_addr, uint8_t reg_addr, uint8_t data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    
    return ret;
}
```

### Lectura simple

```c
esp_err_t i2c_read_byte(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data)
{
    // Escribir dirección del registro
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    
    // Lectura del registro
    i2c_master_start(cmd);  // Repeated start
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, data, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    
    return ret;
}
```

### Funciones de alto nivel (ESP-IDF v5.0+)

```c
#include "driver/i2c_master.h"

i2c_master_bus_handle_t bus_handle;
i2c_device_handle_t dev_handle;

// Configurar bus
i2c_master_bus_config_t bus_config = {
    .i2c_port = I2C_NUM_0,
    .sda_io_num = 6,
    .scl_io_num = 7,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
};
ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));

// Agregar dispositivo
i2c_device_config_t dev_config = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = 0x76,  // BME280
    .scl_speed_hz = 400000,
};
ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config, &dev_handle));

// Escribir
uint8_t write_buf[2] = {0xF4, 0x27};  // Registro, valor
ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, write_buf, 2, -1));

// Leer
uint8_t reg_addr = 0xD0;  // Chip ID
uint8_t read_buf[1];
ESP_ERROR_CHECK(i2c_master_transmit_receive(dev_handle, &reg_addr, 1, read_buf, 1, -1));
```

## Ejemplo completo: BME280

```c
#include "driver/i2c.h"
#include "esp_log.h"

#define BME280_ADDR         0x76
#define BME280_REG_CHIPID   0xD0
#define BME280_REG_CTRL     0xF4
#define BME280_REG_TEMP_MSB 0xFA

#define I2C_MASTER_NUM      I2C_NUM_0
#define I2C_MASTER_SCL_IO   7
#define I2C_MASTER_SDA_IO   6

static const char *TAG = "BME280";

void bme280_init(void)
{
    // Inicializar I2C
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000,
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
    
    // Leer Chip ID
    uint8_t chip_id;
    i2c_read_byte(BME280_ADDR, BME280_REG_CHIPID, &chip_id);
    ESP_LOGI(TAG, "BME280 Chip ID: 0x%02X", chip_id);
    
    // Configurar modo normal
    i2c_write_byte(BME280_ADDR, BME280_REG_CTRL, 0x27);
}

int32_t bme280_read_temperature(void)
{
    uint8_t data[3];
    
    // Leer temperatura (3 bytes)
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BME280_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, BME280_REG_TEMP_MSB, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BME280_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, 3, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    
    int32_t adc_T = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
    
    // Conversión simplificada (sin calibración completa)
    return adc_T / 100;  // Aproximación
}

void app_main(void)
{
    bme280_init();
    
    while (1) {
        int32_t temp = bme280_read_temperature();
        ESP_LOGI(TAG, "Temperatura: %d °C", temp);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
```

## Solución de problemas

### No se detecta dispositivo

1. **Verificar conexiones**: SDA, SCL, VCC, GND
2. **Verificar resistencias pull-up**: medir 3.3V en SDA y SCL con multímetro
3. **Verificar dirección**: usar scanner I²C
4. **Verificar alimentación**: algunos sensores requieren 5V

### Comunicación intermitente

1. **Cables demasiado largos**: usar <30cm
2. **Interferencia electromagnética**: alejar de fuentes de ruido
3. **Capacitancia del bus**: reducir velocidad o usar pull-ups más bajos (2.2kΩ)

### ACK Error

El dispositivo no responde (dirección incorrecta o dispositivo no inicializado).

```c
esp_err_t ret = i2c_master_cmd_begin(...);
if (ret == ESP_ERR_TIMEOUT) {
    ESP_LOGE(TAG, "I2C timeout (dispositivo no responde)");
} else if (ret == ESP_FAIL) {
    ESP_LOGE(TAG, "ACK error (dirección incorrecta?)");
}
```

## Próximos pasos

- [Comunicación SPI](./spi.md)
- [Sensores y transductores](./sensors.md)
- [Lab 1: Sensores I²C](../examples/lab01-i2c-sensors.md)
- [Lab 2: Pantalla OLED](../examples/lab02-oled-display.md)

## Referencias

- [ESP32-C6 I²C Driver](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/peripherals/i2c.html)
- [I²C Specification](https://www.nxp.com/docs/en/user-guide/UM10204.pdf)
- [I²C Pull-up Resistor Calculation](http://www.ti.com/lit/an/slva689/slva689.pdf)
