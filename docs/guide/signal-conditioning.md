# Acondicionamiento de Señal

El acondicionamiento de señal adapta la salida de un sensor para que pueda ser procesada correctamente por el ADC del ESP32-C6.

## ¿Por qué acondicionar señales?

Los sensores pueden producir señales que:
- Están fuera del rango del ADC (0-2.5V)
- Son muy pequeñas (µV o mV)
- Contienen ruido eléctrico
- Tienen offset (voltaje DC no deseado)
- Requieren aislamiento galvánico

## Técnicas de acondicionamiento

### 1. Amplificación

#### Amplificador no inversor con op-amp

Aumenta la amplitud de señales pequeñas:

```
        R2
    ┌────────┐
    │        │
Vin ─┤+       │
     │  741   ├─── Vout
  ┌──┤-       │
  │  └────────┘
  │      │
  └── R1 │
         │
        GND
```

**Ganancia**: $A_v = 1 + \frac{R_2}{R_1}$

**Ejemplo**: Amplificar señal de 0-100mV a 0-2.5V
- Ganancia necesaria: $\frac{2.5V}{0.1V} = 25$
- Si $R_1 = 1k\Omega$, entonces $R_2 = (25-1) \times 1k = 24k\Omega$

**Código de ejemplo**:
```c
// Lectura después de amplificación x25
int adc_raw = adc1_get_raw(ADC1_CHANNEL_0);
float voltage_adc = (adc_raw / 4095.0) * 2.5;
float voltage_original = voltage_adc / 25.0;  // Dividir por ganancia
```

#### Amplificador inversor

```
      R2
   ┌──────┐
   │      │
R1 │   ┌──┤-       ├─── Vout
───┼───┤  │  741   │
       │  ┤+       │
       │  └────────┘
       │      │
      GND    GND
```

**Ganancia**: $A_v = -\frac{R_2}{R_1}$ (invierte la señal)

### 2. Filtrado

#### Filtro paso bajo RC (Low-pass filter)

Elimina ruido de alta frecuencia:

```
Vin ──┬── R ──┬── Vout → ADC
      │       │
      C      GND
      │
     GND
```

**Frecuencia de corte**: $f_c = \frac{1}{2\pi RC}$

**Ejemplo**: Filtrar ruido > 100 Hz
- $C = 1\mu F$, $R = ?$
- $R = \frac{1}{2\pi \times 100 \times 1 \times 10^{-6}} \approx 1.6k\Omega$

#### Filtro paso alto RC (High-pass filter)

Elimina componente DC (offset):

```
         C
Vin ──┬──||──┬── Vout → ADC
      │      │
      R     GND
      │
     GND
```

**Frecuencia de corte**: $f_c = \frac{1}{2\pi RC}$

#### Filtro Notch (rechaza banda)

Elimina frecuencias específicas (p.ej. 50/60 Hz de la red eléctrica).

### 3. Divisor de voltaje

Reduce voltajes mayores al rango del ADC:

```
Vin ──┬── R1
      │
      ├── Vout → ADC (0-2.5V)
      │
      └── R2
      │
     GND
```

**Fórmula**: $V_{out} = V_{in} \times \frac{R_2}{R_1 + R_2}$

**Ejemplo**: Medir batería 12V
- Rango ADC: 0-2.5V
- Factor división: $\frac{12V}{2.5V} = 4.8$
- Si $R_2 = 10k\Omega$, entonces: $R_1 = R_2 \times (4.8 - 1) = 38k\Omega$ (usar 39kΩ)

**Código**:
```c
int adc_raw = adc1_get_raw(ADC1_CHANNEL_0);
float voltage_adc = (adc_raw / 4095.0) * 2.5;
float voltage_battery = voltage_adc * 4.8;  // Multiplicar por factor
ESP_LOGI(TAG, "Batería: %.2f V", voltage_battery);
```

### 4. Offset y escalado

#### Sumador con op-amp

Agrega un voltaje offset:

```
      R1
V1 ───┬─────┐
      │     │
      R2    │   ┌──┤-       ├─── Vout
V2 ───┬─────┼───┤  │  741   │
      │     │   │  ┤+       │
      Rf    │   │  └────────┘
      │     │  GND
     GND   GND
```

**Salida**: $V_{out} = -\left(\frac{R_f}{R_1}V_1 + \frac{R_f}{R_2}V_2\right)$

**Aplicación**: Centrar señal AC (-1V a +1V) en 1.25V (rango ADC: 0.25V a 2.25V)

### 5. Rectificación

#### Rectificador de precisión (onda completa)

Convierte señales AC en DC para medir amplitud:

```
          D1
Vin ──┬───▶|───┬──── R ────┬──── Vout
      │        │           │
      │     ┌──┤-          │
      └─────┤  │  741   ├──┘
            │  ┤+       │
           GND └────────┘
```

Útil para medir señales AC como audio o sensores de vibración.

### 6. Aislamiento galvánico

Separa eléctricamente la entrada de la salida para protección:

#### Optoacoplador
```
Entrada ──┬── R ──┤>──LED  Fototransistor──│
          │       └────────────────────────┘
         GND                                │
                                        Vout → ADC
```

Protege contra sobrevoltajes, ruido de tierra común.

#### Amplificador de aislamiento (ISO124, ADUM)
Para señales analógicas de alta precisión.

## Circuitos prácticos

### Ejemplo 1: Sensor de temperatura LM35 (10mV/°C)

**Problema**: LM35 produce 0-1V para 0-100°C. Queremos usar todo el rango del ADC (0-2.5V).

**Solución**: Amplificador no inversor con ganancia 2.5

```
LM35 ─── 10mV/°C ───┬──┤+ Op-Amp ├─── 0-2.5V → ADC
                    │  └─────────┘
                    │      │
                    └─ 1kΩ │
                           │
                        2.2kΩ
                           │
                          GND
```

Ganancia: $1 + \frac{2.2k}{1k} = 3.2$ (ajustar a 2.5)

**Código**:
```c
int adc_raw = adc1_get_raw(ADC1_CHANNEL_0);
float voltage = (adc_raw / 4095.0) * 2.5;
float temperature = voltage / 0.025;  // 25mV/°C después de amplificar
ESP_LOGI(TAG, "Temperatura: %.1f °C", temperature);
```

### Ejemplo 2: Medición de corriente con sensor Hall ACS712

**Sensor**: ACS712-05A (±5A)
- Salida: 2.5V en corriente cero
- Sensibilidad: 185 mV/A

**Problema**: Rango 0.65V a 4.35V (fuera de rango ADC 0-2.5V)

**Solución**: 
1. Divisor de voltaje para reducir a 0-2.5V
2. Restar offset en software

```
ACS712 ──┬── 22kΩ
         │
         ├── Vout → ADC
         │
         └── 33kΩ
         │
        GND
```

**Código**:
```c
int adc_raw = adc1_get_raw(ADC1_CHANNEL_0);
float voltage_adc = (adc_raw / 4095.0) * 2.5;

// Recuperar voltaje original
float voltage_sensor = voltage_adc * (22 + 33) / 33.0;

// Restar offset y calcular corriente
float current = (voltage_sensor - 2.5) / 0.185;
ESP_LOGI(TAG, "Corriente: %.2f A", current);
```

### Ejemplo 3: ECG/EMG (señal bioeléctrica)

**Problema**: Señal muy pequeña (mV), con offset DC, ruido 50/60 Hz

**Solución completa**:
1. **Amplificador de instrumentación** (INA128): ganancia 100-1000
2. **Filtro paso alto**: 0.5 Hz (eliminar DC)
3. **Filtro paso bajo**: 150 Hz (eliminar ruido alta frecuencia)
4. **Filtro Notch**: 50/60 Hz (eliminar ruido de red)

```
Electrodos → INA128 → Filtro paso alto → Filtro paso bajo → Notch 50Hz → ADC
```

**Alternativa**: Usar módulo AD8232 (ECG front-end integrado)

## Amplificadores operacionales comunes

| Modelo | Tipo | Características |
|--------|------|-----------------|
| LM358 | Dual | Económico, rail-to-rail output |
| TL072 | Dual | Bajo ruido, para audio |
| LM324 | Quad | 4 op-amps, económico |
| OPA2134 | Dual | Alta calidad audio |
| INA128 | Instrumentación | Para señales diferenciales pequeñas |

::: tip Alimentación
Op-amps requieren alimentación dual (±5V) o simple (0-5V). Para ESP32-C6 (3.3V), usa op-amps rail-to-rail como LM358.
:::

## Consideraciones prácticas

### 1. Impedancia de entrada del ADC

El ADC del ESP32-C6 tiene impedancia de entrada finita. Usar buffer (seguidor de voltaje):

```
Sensor ──┤+ Op-Amp ├─── ADC
         └─────────┘
              │
              └─── salida conecta a entrada -
```

Ganancia = 1, pero aísla y presenta alta impedancia al sensor.

### 2. Capacitores de desacople

Agregar cerca de los pines de alimentación del op-amp:
```
VCC ──┬── 100nF ──┬── GND
      │           │
   Op-Amp      100µF
                  │
                 GND
```

### 3. Protección del ADC

**Diodos Zener** para limitar voltaje:
```
Señal ──┬─── R (1kΩ) ──┬─── ADC
        │               │
       D1 (3.3V)       D2
        │               │
       GND             GND
```

### 4. Blindaje (Shielding)

Para cables largos, usa cable blindado conectado a GND para reducir interferencia electromagnética (EMI).

## Herramientas de diseño

- **LTspice**: simulador de circuitos gratuito
- **Calculadoras online**: 
  - [Filter Calculator](http://sim.okawa-denshi.jp/en/Fkeisan.htm)
  - [Op-Amp Calculator](https://www.ti.com/design-resources/design-tools-simulation.html)

## Debugging

### Señal con ruido
- Agregar filtro paso bajo
- Verificar conexiones a tierra
- Usar cables más cortos
- Agregar capacitor de desacople

### Saturación (señal cortada)
- Reducir ganancia del amplificador
- Verificar alimentación del op-amp
- Usar op-amp rail-to-rail

### Offset inesperado
- Usar filtro paso alto
- Calibrar en software
- Verificar tierra común

## Próximos pasos

- [ADC y DAC](./adc-dac.md)
- [Lab 3: ADC analógico](../examples/lab03-adc.md)
- [Sensores y transductores](./sensors.md)

## Referencias

- [Texas Instruments Op-Amp Guide](https://www.ti.com/lit/an/sloa011a/sloa011a.pdf)
- [Analog Devices: Signal Conditioning](https://www.analog.com/en/design-center/landing-pages/001/beginners-guide-to-iiot-ebook.html)
- [ESP32-C6 ADC Characteristics](https://www.espressif.com/sites/default/files/documentation/esp32-c6_datasheet_en.pdf)
