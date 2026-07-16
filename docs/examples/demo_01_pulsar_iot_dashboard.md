# Demo 01 - Pulsar IoT Dashboard

A complete IoT dashboard running entirely on the **DevLab Pulsar ESP32-C6**, demonstrating the integration of embedded web technologies, sensors, storage, RGB lighting, and real-time hardware control.

This project transforms the Pulsar C6 into a self-contained IoT node capable of hosting its own responsive web interface without requiring an external server.

---

## Features

- 🌐 Embedded Web Server
- 📶 Wi-Fi Access Point mode
- 💾 microSD File Manager
- 📂 Read, create, edit, append and delete files
- 📈 Real-time BMI270 visualization
- 👆 Capacitive Touch monitoring using interrupts
- 🎨 RGB control for 26 NeoPixels
- 🔆 PWM brightness control for the status LED
- 📡 REST API for hardware interaction
- 📱 Responsive dashboard for PC, tablet and mobile devices

---

## Hardware Used

| Device | Description |
|---------|-------------|
| DevLab Pulsar ESP32-C6 | Main controller |
| microSD Card | File storage |
| BMI270 | Accelerometer & Gyroscope |
| Capacitive Touch Sensor | Digital interrupt input |
| 5×5 NeoPixel Matrix | RGB lighting |
| Onboard NeoPixel | Status RGB LED |
| Status LED | PWM brightness demonstration |

---

# Dashboard Overview

The embedded web interface is divided into several functional modules.

## Touch Monitor

Displays the state of the capacitive touch sensor.

Features

- Circular status indicator
- Real-time state
- Interrupt counter

The indicator changes from **gray** to **blue** whenever a touch event is detected.

---

## BMI270 Monitor

Displays live motion data acquired through the I²C interface.

Displayed variables:

- Acceleration X
- Acceleration Y
- Acceleration Z
- Gyroscope X
- Gyroscope Y
- Gyroscope Z

The values are refreshed continuously without reloading the page.

---

## NeoPixel Controller

Allows changing the color of all 26 NeoPixels.

Features

- RGB color picker
- Quick color buttons
- Brightness control
- Instant update

---

## Status LED

Demonstrates PWM generation on the ESP32-C6.

Features

- Adjustable brightness
- Smooth breathing animation
- Touch activity indication
- System status feedback

---

## microSD File Manager

A complete embedded file manager.

Supported operations

- Read directory
- Browse files
- Read file
- Create file
- Overwrite file
- Append text
- Delete file

The terminal displays every operation performed by the ESP32-C6.

---

# REST API

The dashboard communicates with the ESP32-C6 using a REST API.

| Method | Endpoint | Description |
|---------|----------|-------------|
| GET | /api/state | Returns system status |
| GET | /api/files | Lists files |
| GET | /api/file | Reads a file |
| POST | /api/file | Creates or writes a file |
| DELETE | /api/file | Deletes a file |
| GET | /api/neopixel | Updates NeoPixel color |
| GET | /api/status-led | Changes PWM brightness |

---

# Project Architecture

```
                ┌─────────────────────┐
                │     Web Browser     │
                └──────────┬──────────┘
                           │ HTTP REST
                           │
                  ┌────────▼─────────┐
                  │   ESP32-C6 AP    │
                  │ Embedded Server  │
                  └────────┬─────────┘
                           │
      ┌───────────┬────────┼──────────┬────────────┐
      │           │        │          │            │
      ▼           ▼        ▼          ▼            ▼
   Touch       BMI270    microSD   NeoPixels   Status LED
 Interrupt      I²C         SPI       D2         PWM
```

---

# Pin Assignment

| Peripheral | Pin |
|------------|-----|
| NeoPixels | D2 |
| Status LED | D4 |
| Touch Sensor | D5 |
| microSD SCK | D13 |
| microSD MISO | D12 |
| microSD MOSI | D11 |
| microSD CS | D10 |
| BMI270 SDA | A4 |
| BMI270 SCL | A5 |

---

# Required Libraries

- WiFi
- WebServer
- SPI
- SD
- FS
- Wire
- Adafruit NeoPixel
- SparkFun BMI270 Arduino Library

---

# Folder Structure

```
demo01-pulsar-iot-dashboard/
│
├── demo01-pulsar-iot-dashboard.ino
├── README.md
└── img/
    ├── dashboard.png
    ├── architecture.png
    └── hardware.jpg
```

---

# Future Improvements

Some ideas for extending this project include:

- WebSockets instead of polling
- OTA firmware update
- File upload from browser
- Download files from microSD
- Create folders
- Rename files
- Drag & Drop file manager
- Sensor logging to CSV
- Live plotting of IMU data
- MQTT client
- JSON configuration manager
- Authentication
- Dark/Light themes

---

# License

MIT License

---

Developed for the **DevLab Pulsar ESP32-C6** platform.