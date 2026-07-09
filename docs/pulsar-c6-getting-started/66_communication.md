# Comunicación

Aprovecha el potencial de comunicación de la placa NANO ESP32C6 mediante distintos protocolos e interfaces. Aprende a configurar y usar Wi-Fi, Bluetooth y comunicación serie para conectarte con otros dispositivos y redes.

## Wi-Fi

Aprende a configurar y usar la comunicación Wi-Fi en la placa DualMCU ONE.

``` 
import machine
import network

wlan = network.WLAN(network.STA_IF)
wlan.active(True)
wlan.connect('your-ssid', 'your-password')

while not wlan.isconnected():
    pass

print('Connected to Wi-Fi')

# Check the IP address
print(wlan.ifconfig())
```

## Bluetooth

Explora las capacidades de comunicación Bluetooth y aprende a conectarte con dispositivos Bluetooth.

Código de escaneo tipo sniffer

``` python
import bluetooth
import time

# Initialize Bluetooth
ble = bluetooth.BLE()
ble.active(True)

# Helper function to convert memoryview to MAC address string
def format_mac(addr):
    return ':'.join('{:02x}'.format(b) for b in addr)

# Helper function to parse device name from advertising data
def decode_name(data):
    i = 0
    length = len(data)
    while i < length:
        ad_length = data[i]
        ad_type = data[i + 1]
        if ad_type == 0x09:  # Complete Local Name
            return str(data[i + 2:i + 1 + ad_length], 'utf-8')
        elif ad_type == 0x08:  # Shortened Local Name
            return str(data[i + 2:i + 1 + ad_length], 'utf-8')
        i += ad_length + 1
    return None

# Global counter for devices found
devices_found = 0
max_devices = 10  # Limit to 10 devices

# Callback function to handle advertising reports
def bt_irq(event, data):
    global devices_found
    if event == 5:  # event 5 is for advertising reports
        if devices_found >= max_devices:
            ble.gap_scan(None)  # Stop scanning
            print("Scan stopped, limit reached.")
            return

        addr_type, addr, adv_type, rssi, adv_data = data
        mac_addr = format_mac(addr)
        device_name = decode_name(adv_data)
        if device_name:
            print(f"Device found: {mac_addr} (RSSI: {rssi}) Name: {device_name}")
        else:
            print(f"Device found: {mac_addr} (RSSI: {rssi}) Name: Unknown")

        devices_found += 1  # Increment counter

        if devices_found >= max_devices:
            ble.gap_scan(None)  # Stop scanning
            print("Scan stopped, limit reached.")

# Set the callback function
ble.irq(bt_irq)

# Start active scanning
ble.gap_scan(10000, 30000, 30000, True)  # Active scan for 10 seconds with interval and window of 30ms

# Keep the program running to allow the callback to be processed
while True:
    time.sleep(1)
```

## Serial

Aprende sobre comunicación serie y cómo comunicarte con otros dispositivos mediante puertos seriales.
