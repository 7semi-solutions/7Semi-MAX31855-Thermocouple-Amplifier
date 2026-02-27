#ifndef _7SEMI_MAX31855_H_
#define _7SEMI_MAX31855_H_

/**
 * 7Semi MAX31855 Thermocouple Library
 * 
 * - Supports hardware SPI
 * - Reads thermocouple temperature (14-bit signed)
 * - Reads internal cold junction temperature
 * - Detects fault conditions
 * - Compatible with Arduino / ESP32 / STM32
 * 
 * Resolution:
 * - Thermocouple  = 0.25°C per LSB
 * - Internal Temp = 0.0625°C per LSB
 */

#include <Arduino.h>
#include <SPI.h>

#define _7SEMI_MAX31855_VERSION "1.0.0"

class MAX31855_7Semi {
public:


/**
 * Thermocouple Types
 *
 * - TYPE_K  = Nickel-Chromium / Nickel-Alumel
 *             Range: -200°C to +1350°C
 *             Most commonly used general-purpose type
 *
 * - TYPE_J  = Iron / Constantan
 *             Range: -210°C to +760°C
 *             Good for older industrial systems
 *
 * - TYPE_T  = Copper / Constantan
 *             Range: -200°C to +400°C
 *             High accuracy at low temperatures
 *
 * - TYPE_E  = Chromel / Constantan
 *             Range: -200°C to +900°C
 *             High output voltage, good sensitivity
 *
 * - TYPE_N  = Nicrosil / Nisil
 *             Range: -200°C to +1300°C
 *             More stable than K at high temperatures
 *
 * - TYPE_R  = Platinum / Platinum-Rhodium (13%)
 *             Range: 0°C to +1600°C
 *             High accuracy laboratory applications
 *
 * - TYPE_S  = Platinum / Platinum-Rhodium (10%)
 *             Range: 0°C to +1600°C
 *             Similar to Type R, industrial standard
 *
 * - TYPE_B  = Platinum-Rhodium (30% / 6%)
 *             Range: +600°C to +1700°C
 *             Very high temperature furnace applications
 *
 * Note:
 * - MAX31855 chip version determines supported type.
 * - This enum does NOT change internal scaling.
 * - Scaling is fixed:
 *     Thermocouple  = 0.25°C per LSB
 *     Internal Temp = 0.0625°C per LSB
 */
enum ThermocoupleType : uint8_t {
    TYPE_K,
    TYPE_J,
    TYPE_T,
    TYPE_E,
    TYPE_N,
    TYPE_R,
    TYPE_S,
    TYPE_B
};
    /**
     * Fault codes from MAX31855
     * - FAULT_NONE      = No error
     * - FAULT_OPEN      = Thermocouple disconnected
     * - FAULT_SHORT_GND = Short to ground
     * - FAULT_SHORT_VCC = Short to VCC
     */
    enum FaultCode : uint8_t {
        FAULT_NONE       = 0,
        FAULT_OPEN       = 1,
        FAULT_SHORT_GND  = 2,
        FAULT_SHORT_VCC  = 4
    };

    /**
     * Temperature Units
     * - CELSIUS
     * - FAHRENHEIT
     * - KELVIN
     */
    enum Unit : uint8_t {
        CELSIUS,
        FAHRENHEIT,
        KELVIN
    };

    /**
     * Constructor
     * - Default configuration
     */
     /**
     * Constructor with thermocouple type
     */
     MAX31855_7Semi(ThermocoupleType type = TYPE_K);

    /**
     * begin()
     * - Initializes SPI communication
     * - cs           = Chip Select pin
     * - spiPort      = SPI bus reference
     * - clockSpeed   = SPI clock frequency
     * - sck/miso/mosi = Optional custom SPI pins (ESP32)
     */
    bool begin(uint8_t cs = 10,
               SPIClass &spiPort = SPI,
               uint32_t clockSpeed = 1000000,
               uint8_t sck = 255,
               uint8_t miso = 255,
               uint8_t mosi = 255);

    /**
     * readRaw()
     * - Reads sensor data
     * - Returns false if fault detected
     */
    bool readRaw();

    /**
     * Temperature Getters
     */
    float getTemperatureC() const;
    float getTemperatureF() const;
    float getTemperatureK() const;

    /**
     * Internal cold junction temperature
     */
    float getInternalTemperatureC() const;

    /**
     * Returns human readable fault description
     */
    const char* getFault() const;

private:
    uint8_t cs_pin = 10;
    uint32_t spi_clock = 1000000;
    SPIClass* spi = nullptr;

    ThermocoupleType thermocouple_type = TYPE_K;

    int16_t raw_temperature_c = 0;
    int16_t raw_cold_junction = 0;
    uint8_t fault = FAULT_NONE;

    uint32_t read();
};

#endif