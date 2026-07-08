# Documentación Migrada desde Sphinx

Esta sección conserva el contenido fuente que estaba en `sphinx/src/source`.
Sirve como base para revisar, traducir y reorganizar los instructivos, imágenes
y ejemplos de código sin depender de Sphinx.

## Páginas del Manual Original

- [Acerca de PULSAR C6](./00_about.md)
- [Configuración de escritorio](./01_setup.md)
- [Librerías MicroPython](./02_lib.md)
- [Primeros pasos con ESP-IDF](./02_1_esp-idf.md)
- [Tarjeta, esquemático y pinout](./03_pinout.md)
- [GPIO](./11_gpio.md)
- [ADC](./22_adc.md)
- [I2C](./33_i2c.md)
- [SPI y Micro SD](./44_spi.md)
- [WS2812 / NeoPixel](./55_neop.md)
- [Comunicación](./66_communication.md)
- [Conector JST SH](./jst.md)
- [Entorno MicroPython y Arduino IDE](./env.md)
- [Reporte de errores](./report.md)

## Recursos Migrados

Los recursos estáticos de Sphinx fueron copiados a `docs/public/sphinx-static`.
Las páginas usan rutas como `/sphinx-static/nanoc6/top.png`, por lo que las
imágenes, PDF y capturas quedan disponibles directamente desde VitePress.
