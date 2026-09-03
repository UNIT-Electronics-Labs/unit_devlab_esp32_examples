import { defineConfig } from 'vitepress'

export default defineConfig({
  markdown: {
    math: true,
  },
  base: process.env.BASE_PATH || '/',
  title: 'ESP32-C6 Pulsar Labs',
  description: 'Documentación de adquisición de datos e IoT con ESP32-C6 y Pulsar C6.',
  lang: 'es-MX',
  cleanUrls: true,
  themeConfig: {
    nav: [
      { text: 'Inicio', link: '/' },
      { text: 'Guías', link: '/guide/esp32c6' },
      { text: 'Laboratorios', link: '/examples/' },
      { text: 'Primeros pasos con Pulsar C6', link: '/pulsar-c6-getting-started/' }
    ],
    sidebar: [
      {
        text: 'Configuración',
        items: [
          { text: 'ESP32-C6 & Pulsar C6', link: '/guide/esp32c6' },
          { text: 'Configuración de escritorio', link: '/pulsar-c6-getting-started/01_setup' },
          {
            text: 'Código y entornos',
            collapsed: false,
            items: [
              { text: 'Arduino IDE (C/C++)', link: '/pulsar-c6-getting-started/02_0_duino' },
              { text: 'MicroPython (Thonny)', link: '/pulsar-c6-getting-started/env' },
              { text: 'Librerías MicroPython', link: '/pulsar-c6-getting-started/02_lib' },
              { text: 'ESP-IDF (Espressif C/C++)', link: '/guide/esp-idf' },
              { text: 'ESP-IDF avanzado', link: '/pulsar-c6-getting-started/02_1_esp-idf' }
            ]
          }
        ]
      },
      {
        text: 'Primeros pasos con Pulsar C6',
        items: [
          { text: 'Índice', link: '/pulsar-c6-getting-started/' },
          { text: 'Acerca de PULSAR C6', link: '/pulsar-c6-getting-started/00_about' },
          { text: 'Pinout y esquemático', link: '/pulsar-c6-getting-started/03_pinout' },
          { text: 'GPIO', link: '/pulsar-c6-getting-started/11_gpio' },
          { text: 'ADC', link: '/pulsar-c6-getting-started/22_adc' },
          { text: 'I²C', link: '/pulsar-c6-getting-started/33_i2c' },
          { text: 'SPI y Micro SD', link: '/pulsar-c6-getting-started/44_spi' },
          { text: 'WS2812 / NeoPixel', link: '/pulsar-c6-getting-started/55_neop' },
          { text: 'Comunicación', link: '/pulsar-c6-getting-started/66_communication' },
          { text: 'Conector JST SH', link: '/pulsar-c6-getting-started/jst' },
          { text: 'Reporte de errores', link: '/pulsar-c6-getting-started/report' }
        ]
      },
      {
        text: 'Conceptos Teóricos',
        items: [
          { text: 'Adquisición de Datos', link: '/guide/data-acquisition' },
          { text: 'Sensores y Transductores', link: '/guide/sensors' },
          { text: 'ADC y DAC', link: '/guide/adc-dac' },
          { text: 'Acondicionamiento de Señal', link: '/guide/signal-conditioning' },
          { text: 'Comunicación I²C', link: '/guide/i2c' },
          { text: 'Comunicación SPI', link: '/guide/spi' },
          { text: 'Wi-Fi y Bluetooth LE', link: '/guide/connectivity' },
          { text: 'Zigbee', link: '/guide/zigbee' },
          { text: 'Plataformas IoT', link: '/guide/iot-platforms' }
        ]
      },
      {
        text: 'Prácticas de Laboratorio',
        items: [
          { text: 'Índice', link: '/examples/' },
          { text: 'Lab 1: Sensores I²C', link: '/examples/lab01-i2c-sensors' },
          { text: 'Lab 2: Pantalla OLED', link: '/examples/lab02-oled-display' },
          { text: 'Lab 3: ADC Analógico', link: '/examples/lab03-adc' },
          { text: 'Lab 4: Publicación Wi-Fi', link: '/examples/lab04-wifi' },
          { text: 'Lab 5: Bluetooth LE', link: '/examples/lab05-ble' },
          { text: 'Lab 6: Zigbee', link: '/examples/lab06-zigbee-basic' },
          { text: 'Lab 7: PULSAR Stick I²C y Zigbee', link: '/examples/labs07-zigbee-pulsar-stick' }
        ]
      },
      {
        text: 'Kubi',
        items: [
          { text: 'Instalación y configuración', link: 'kubi/kubi.md' }
        ]
      }
    ],
    search: {
      provider: 'local',
      options: {
        translations: {
          button: {
            buttonText: 'Buscar',
            buttonAriaLabel: 'Buscar'
          },
          modal: {
            displayDetails: 'Mostrar detalles',
            resetButtonTitle: 'Limpiar búsqueda',
            backButtonTitle: 'Cerrar búsqueda',
            noResultsText: 'Sin resultados',
            footer: {
              selectText: 'seleccionar',
              selectKeyAriaLabel: 'entrar',
              navigateText: 'navegar',
              navigateUpKeyAriaLabel: 'flecha arriba',
              navigateDownKeyAriaLabel: 'flecha abajo',
              closeText: 'cerrar',
              closeKeyAriaLabel: 'escape'
            }
          }
        }
      }
    },
    outline: {
      level: [2, 3]
    },
    outlineTitle: 'En esta página',
    darkModeSwitchLabel: 'Apariencia',
    lightModeSwitchTitle: 'Cambiar a tema claro',
    darkModeSwitchTitle: 'Cambiar a tema oscuro',
    sidebarMenuLabel: 'Menú',
    returnToTopLabel: 'Volver arriba',
    langMenuLabel: 'Cambiar idioma',
    skipToContentLabel: 'Saltar al contenido',
    docFooter: {
      prev: 'Página anterior',
      next: 'Página siguiente'
    },
    footer: {
      message: 'Prácticas de adquisición de datos e IoT con ESP32-C6.',
      copyright: 'MIT'
    }
  }
})
