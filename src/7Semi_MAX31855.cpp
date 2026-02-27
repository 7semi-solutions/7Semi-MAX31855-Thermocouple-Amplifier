#include "7Semi_MAX31855.h"

/**
 * MAX31855_7Semi Constructor
 * - Stores selected thermocouple type
 * - Default is TYPE_K
 */
MAX31855_7Semi::MAX31855_7Semi(ThermocoupleType thermocoupleType) {
    thermocouple_type = thermocoupleType;
}

/**
 * begin()
 * - Initializes SPI communication
 * - Configures CS pin
 * - Supports ESP32 custom SPI pins
 * - Uses SPI Mode 0 (MAX31855 requirement)
 * - Returns true if initialized
 */
bool MAX31855_7Semi::begin(uint8_t cs,
                           SPIClass &spiPort,
                           uint32_t clockSpeed,
                           uint8_t sck,
                           uint8_t miso,
                           uint8_t mosi)
{
    cs_pin = cs;
    spi_clock = clockSpeed;
    spi = &spiPort;

    pinMode(cs_pin, OUTPUT);
    digitalWrite(cs_pin, HIGH);

#if defined(ARDUINO_ARCH_ESP32)
    if (sck != 255 && miso != 255) {
        spi->begin(sck, miso, mosi);
    } else {
        spi->begin();
    }
#else
    spi->begin();
#endif

    return true;
}

/**
 * readRaw()
 *
 * - Reads full 32-bit data frame from MAX31855
 * - Extracts:
 *     • Thermocouple temperature (14-bit signed)
 *     • Internal cold junction temperature (12-bit signed)
 *     • Fault flags
 *
 * Data Format:
 * - D31–D18 = Thermocouple temperature
 * - D16     = Fault indicator
 * - D15–D4  = Internal temperature
 * - D2–D0   = Fault detail bits
 *
 * Returns:
 * - true  = Valid reading
 * - false = Fault detected (use getFault())
 */
bool MAX31855_7Semi::readRaw()
{
    uint32_t raw = read();

    /* Fault bit = D16 */
    if (raw & 0x00010000) {
        fault = raw & 0x07;   // D2–D0
        return false;
    }

    fault = FAULT_NONE;

    /* Thermocouple temperature (D31–D18) */
    raw_temperature_c = (raw >> 18) & 0x3FFF;
    if (raw_temperature_c & 0x2000) {
        raw_temperature_c |= 0xC000;   // Sign extend
    }

    /* Internal temperature (D15–D4) */
    raw_cold_junction = (raw >> 4) & 0x0FFF;
    if (raw_cold_junction & 0x0800) {
        raw_cold_junction |= 0xF000;   // Sign extend
    }

    return true;
}

/**
 * read()
 * - Performs 32-bit SPI read
 * - MSB first
 * - MAX31855 requires Mode 0
 */
uint32_t MAX31855_7Semi::read()
{
    uint32_t data = 0;

    spi->beginTransaction(SPISettings(spi_clock, MSBFIRST, SPI_MODE0));

    digitalWrite(cs_pin, LOW);
    delayMicroseconds(1);

    for (int i = 0; i < 4; i++) {
        data <<= 8;
        data |= spi->transfer(0x00);
    }

    digitalWrite(cs_pin, HIGH);

    spi->endTransaction();

    return data;
}

/**
 * getTemperatureC()
 *
 * - Converts raw thermocouple reading to Celsius
 * - Uses MAX31855 fixed resolution
 * - Thermocouple resolution = 0.25°C per LSB
 * - Value is already linearized by hardware
 *
 * Notes:
 * - Scaling is independent of thermocouple type enum
 * - Chip variant (K, J, T, etc.) determines calibration
 * - readRaw() must be called before using this function
 *
 * Returns:
 * - Thermocouple temperature in degrees Celsius
 */
float MAX31855_7Semi::getTemperatureC() const
{
    float scale = 0.25f;

    switch (thermocouple_type)
    {
        case TYPE_K:
        case TYPE_J:
        case TYPE_T:
        case TYPE_E:
        case TYPE_N:
        case TYPE_R:
        case TYPE_S:
        case TYPE_B:
            scale = 0.25f;
            break;
    }

    return raw_temperature_c * scale;
}

/**
 * getTemperatureF()
 *
 * - Converts thermocouple temperature to Fahrenheit
 * - Internally uses getTemperatureC()
 *
 * Formula:
 * - °F = (°C × 9/5) + 32
 *
 * Notes:
 * - readRaw() must be called before using this function
 *
 * Returns:
 * - Thermocouple temperature in degrees Fahrenheit
 */
 float MAX31855_7Semi::getTemperatureF() const
 {
    return getTemperatureC() * 9.0f / 5.0f + 32.0f;
 } 

/**
 * getTemperatureK()
 *
 * - Converts thermocouple temperature to Kelvin
 * - Internally uses getTemperatureC()
 *
 * Formula:
 * - K = °C + 273.15
 *
 * Notes:
 * - readRaw() must be called before using this function
 *
 * Returns:
 * - Thermocouple temperature in Kelvin
 */
float MAX31855_7Semi::getTemperatureK() const
{
    return getTemperatureC() + 273.15f;
}

/**
 * getInternalTemperatureC()
 *
 * - Returns internal cold junction temperature
 * - Used for cold junction compensation
 *
 * Resolution:
 * - 0.0625°C per LSB
 *
 * Notes:
 * - Value is read from D15–D4 of MAX31855 frame
 * - readRaw() must be called before using this function
 *
 * Returns:
 * - Internal reference temperature in degrees Celsius
 */
float MAX31855_7Semi::getInternalTemperatureC() const
{
    return raw_cold_junction * 0.0625f;
}

/**
 * getFault()
 * - Returns readable fault description
 */
const char* MAX31855_7Semi::getFault() const {
    switch (fault) {
        case FAULT_NONE:      return "No Fault";
        case FAULT_OPEN:      return "Open Thermocouple";
        case FAULT_SHORT_GND: return "Short to GND";
        case FAULT_SHORT_VCC: return "Short to VCC";
        default:              return "Unknown Fault";
    }
}