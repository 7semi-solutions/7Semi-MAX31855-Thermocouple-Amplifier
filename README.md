# 7Semi MAX31855 Arduino Library

Arduino library for the MAX31855 Thermocouple-to-Digital Converter.

Supports SPI communication to read:

- Thermocouple temperature

- Internal cold junction temperature

- Fault conditions

Designed for Arduino, ESP32 and other SPI-capable platforms.

---

##  Features

- SPI communication (Mode 0)
- Reads thermocouple temperature (°C / °F / K)
- Reads internal cold junction temperature
- Fault detection:
  - Open thermocouple
  - Short to GND
  - Short to VCC

---

##  Connections 

###  I²C Connection (Most Common)

| MAX31855 Pin | MCU Pin  | Notes                    |
| ------------ | -------- | ------------------------ |
| VCC          | 3.3V     | **3.3V/5V**            |
| GND          | GND      | Common ground            |
| SCK          | SPI SCK  | Clock                    |
| DO           | SPI MISO | Data to MCU              |
| CS           | GPIO     | Chip Select (Active LOW) |


**I@C Notes**
- SPI Mode: MODE0
- Recommended SPI speed: 1 MHz

---

##  Installation

### Arduino Library Manager
1. Open **Arduino IDE**
2. Go to **Library Manager**
3. Search for **7Semi MAX31855**
4. Install

### Manual
1. Download this repository as ZIP
2. Arduino IDE → **Sketch → Include Library → Add .ZIP Library**

---

## ⚡ Quick Start

```cpp
#include <7Semi_MAX31855.h>

#define MAX31855_CS 5

MAX31855_7Semi thermo;

void setup() {
  Serial.begin(115200);
  SPI.begin();

  if (!thermo.begin(MAX31855_CS)) {
    Serial.println("Sensor init failed!");
    while (1);
  }

  Serial.println("MAX31855 Initialized");
}

void loop() {

  if (!thermo.readRaw()) {
    Serial.print("Fault: ");
    Serial.println(thermo.getFault());
  } else {
    Serial.print("Temperature: ");
    Serial.print(thermo.getTemperatureC());
    Serial.print(" °C | ");

    Serial.print(thermo.getTemperatureF());
    Serial.print(" °F | ");

    Serial.print(thermo.getTemperatureK());
    Serial.println(" K");

    Serial.print("Internal Temp: ");
    Serial.print(thermo.getInternalTemperatureC());
    Serial.println(" °C");
  }

  delay(1000);
}
```

---

## Initialization Options

### Default:
``` cpp
thermo.begin(cs_pin);

```

### Advanced:

```cpp
 thermo.begin(cs_pin, SPI, 1000000);
```

Parameters:

cs_pin → Chip Select GPIO

SPI → SPI bus reference

1000000 → SPI clock (1 MHz recommended)

### For ESP32 custom pins:
``` cpp
thermo.begin(cs_pin, SPI, 1000000, sck_pin, miso_pin, mosi_pin);
```

---

## Fault Handling

If readRaw() returns false, use:

```cpp
thermo.getFault();

```

### Possible fault messages:

- No Fault

- Open Thermocouple

- Short to GND

- Short to VCC

### Temperature Functions
| Function                  | Description        |
| ------------------------- | ------------------ |
| getTemperatureC()         | Celsius            |
| getTemperatureF()         | Fahrenheit         |
| getTemperatureK()         | Kelvin             |
| getInternalTemperatureC() | Cold junction temp |
