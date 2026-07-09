<script setup>
import { withBase } from 'vitepress'
</script>

# SPI (Serial Peripheral Interface)

## Descripción general de SPI

SPI (Serial Peripheral Interface) es un bus de comunicación síncrono, full-duplex y maestro-esclavo. Se usa comúnmente para conectar microcontroladores con periféricos como sensores, pantallas y memorias. La placa DualMCU ONE cuenta con capacidades SPI para interactuar con una amplia gama de dispositivos.

## SDCard SPI

<div class="warning">

<div class="title">

Advertencia

</div>

Asegúrate de que la Micro SD contenga datos. Se recomienda guardar varios archivos previamente para facilitar las pruebas. Formatea la tarjeta Micro SD en FAT32 antes de usarla con el ESP32-C6.

</div>

<div id="figura-micro-sd-card">

<figure>
<img :src="withBase('/sphinx-static/Micro-SD-Card-Pinout.png')" class="align-center"
style="width:40.0%" alt="Micro-SD-Card-Pinout.png" />
<figcaption>Pinout de tarjeta Micro SD</figcaption>
</figure>

</div>

<div id="figura-micro-sd-card-reader">

<figure>
<img :src="withBase('/sphinx-static/Lector-Micro-SD.jpg')" class="align-center"
style="width:40.0%" alt="Lector-Micro-SD.jpg" />
<figcaption>Lector externo de tarjeta Micro SD</figcaption>
</figure>

</div>

Las conexiones son las siguientes:

Esta tabla muestra las conexiones entre la tarjeta SD y los pines GPIO del ESP32-C6.

| SD Card | ESP32C6            | PIN |
|---------|--------------------|-----|
| D2      |                    |     |
| D3      | SS (Slave Select)  | 19  |
| CMD     | MOSI               | 7   |
| VSS     | GND                |     |
| VDD     | 3.3V               |     |
| CLK     | SCK (Serial Clock) | 6   |
| VSS     | GND                |     |
| D0      | MISO               | 2   |
| D1      |                    |     |

Conexiones HSPI

### MicroPython

La librería `sdcard.py` para MicroPython en ESP32 y RP2040 es compatible con tarjetas SD mediante comunicación SPI.

**Instalación**

1.  Abre [Thonny](https://thonny.org/).
2.  Copia la librería desde [sdcard.py](https://github.com/UNIT-Electronics-MX/unit_devlab_micropython_libraries/blob/main/software/sdcard/sdcard.py).
3.  Pega la librería en un archivo nuevo en Thonny y guárdala como `sdcard.py` en tu dispositivo.

### C++

La librería `SD.h` de Arduino es compatible con tarjetas SD usando comunicación SPI.

**Instalación**

1.  Abre Arduino IDE.
2.  Ve a **Programa > Incluir biblioteca > Administrar bibliotecas**.

3.  Busca `SD` y haz clic en **Instalar**. También puedes usar la librería SD incluida con Arduino IDE.

### MicroPython

``` python
import machine
import os
from sdcard import SDCard

# Definir pines para SPI y SD
MOSI_PIN = 7
MISO_PIN = 2
SCK_PIN = 6
CS_PIN = 19

# Inicializar SPI
spi = machine.SPI(1, baudrate=500000, polarity=0, phase=0,
                  sck=machine.Pin(SCK_PIN),
                  mosi=machine.Pin(MOSI_PIN),
                  miso=machine.Pin(MISO_PIN))

# Inicializar tarjeta SD
sd = SDCard(spi, machine.Pin(CS_PIN))

# Montar la SD en el sistema de archivos
os.mount(sd, "/sd")

# Listar archivos y directorios en la SD
print("Archivos en la SD:")
print(os.listdir("/sd"))
```

### C++

``` c++
#include <SPI.h>
#include <SD.h>

// Pines SPI (ajusta según tu placa si es necesario)
#define MOSI_PIN 7
#define MISO_PIN 2
#define SCK_PIN 6
#define CS_PIN 19

File myFile;

void setup() {
  Serial.begin(115200);
  while (!Serial) ; // Esperar a que el puerto serie esté listo

  // Configurar los pines SPI manualmente si tu placa lo requiere
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);

  Serial.println("Inicializando tarjeta SD...");

  if (!SD.begin(CS_PIN)) {
    Serial.println("Error al inicializar la tarjeta SD.");
    return;
  }

  Serial.println("Tarjeta SD inicializada correctamente.");

  // Listar archivos
  Serial.println("Archivos en la SD:");
  listDir(SD, "/", 0);

  // Crear y escribir en el archivo
  myFile = SD.open("/test.txt", FILE_WRITE);
  if (myFile) {
    myFile.println("Hola, Arduino en SD!");
    myFile.println("Esto es una prueba de escritura.");
    myFile.close();
    Serial.println("Archivo escrito correctamente.");
  } else {
    Serial.println("Error al abrir test.txt para escribir.");
  }

  // Leer el archivo
  myFile = SD.open("/test.txt");
  if (myFile) {
    Serial.println("\nContenido del archivo:");
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
  } else {
    Serial.println("Error al abrir test.txt para lectura.");
  }

  // Volver a listar archivos
  Serial.println("\nArchivos en la SD después de la escritura:");
  listDir(SD, "/", 0);
}

void loop() {
  // Nada en el loop
}

// Función para listar archivos y carpetas
void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Error al abrir el directorio");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("No es un directorio");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    Serial.print("  ");
    Serial.print(file.name());
    if (file.isDirectory()) {
      Serial.println("/");
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("\t\t");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}
```

### esp-idf

``` c
#include <string.h>
#include <sys/stat.h>
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

#define MOUNT_POINT "/sdcard"

#define PIN_NUM_MISO  CONFIG_EXAMPLE_PIN_MISO
#define PIN_NUM_MOSI  CONFIG_EXAMPLE_PIN_MOSI
#define PIN_NUM_CLK   CONFIG_EXAMPLE_PIN_CLK
#define PIN_NUM_CS    CONFIG_EXAMPLE_PIN_CS

static const char *TAG = "SDCARD";

void app_main(void)
{
    esp_err_t ret;
    sdmmc_card_t *card;

    ESP_LOGI(TAG, "Initializing SD card...");

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 3,
        .allocation_unit_size = 16 * 1024
    };

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();

    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };

    ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init SPI bus.");
        return;
    }

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

    ret = esp_vfs_fat_sdspi_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount filesystem.");
        return;
    }

    ESP_LOGI(TAG, "Filesystem mounted.");

    const char *file_path = MOUNT_POINT"/test.txt";
    FILE *f = fopen(file_path, "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing.");
        return;
    }

    fprintf(f, "Hello from ESP32!\n");
    fclose(f);
    ESP_LOGI(TAG, "File written.");

    f = fopen(file_path, "r");
    if (f) {
        char line[64];
        fgets(line, sizeof(line), f);
        fclose(f);
        ESP_LOGI(TAG, "Read from file: '%s'", line);
    } else {
        ESP_LOGE(TAG, "Failed to read file.");
    }

    esp_vfs_fat_sdcard_unmount(MOUNT_POINT, card);
    spi_bus_free(host.slot);
    ESP_LOGI(TAG, "Card unmounted.");
}
```

<figure>
<img :src="withBase('/sphinx-static/menuconfig.png')" class="align-center"
style="width:90.0%" alt="menuconfig.png" />
<figcaption>Configuración SD SPI en ESP-IDF Menuconfig</figcaption>
</figure>
