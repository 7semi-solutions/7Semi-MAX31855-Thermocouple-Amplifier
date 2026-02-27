/**
 * 7Semi MAX31855 Thermocouple Example
 *
 * Library: 7Semi MAX31855
 * Version: 1.0.0
 *
 * Description:
 * - Reads thermocouple temperature
 * - Displays temperature in Celsius, Fahrenheit, and Kelvin
 * - Displays internal cold junction temperature
 * - Detects and reports fault conditions
 *
 * Hardware Requirements:
 * - MAX31855 Thermocouple-to-Digital Converter
 * - K-type (or supported type) thermocouple
 *
 * SPI Wiring:
 * - VCC → 3.3V
 * - GND → GND
 * - SCK → SPI SCK
 * - SO  → SPI MISO
 * - CS  → GPIO 10 (configurable)
 *
 * Notes:
 * - SPI Mode: MODE0
 * - Recommended SPI clock: 1 MHz
 * - CS must remain LOW during full 32-bit transaction
 * - Use 3.3V logic level
 */
#include <7Semi_MAX31855.h>

/**
 * Create MAX31855 object
 *
 * - Default thermocouple type: TYPE_K
 * - Type enum is informational (scaling is hardware fixed)
 */
MAX31855_7Semi thermo(MAX31855_7Semi::TYPE_K);

void setup()
{
    Serial.begin(115200);


    /**
    * ARDUINO UNO SPI Default Pins:
     * - SCK  = 13
     * - MISO = 12
     * - MOSI = 11
     * - CS   = 10 (configurable)
     *
     * ESP32 VSPI Default Pins:
     * - SCK  = 18
     * - MISO = 19
     * - MOSI = 23
     * - CS   = 5
     *
     * Configuration:
     * - SPI Mode      = MODE0
     * - Clock Speed   = 1 MHz
     *
     * Parameters:
     * - 10        → Chip Select (CS)
     * - SPI       → SPI bus reference
     * - 1000000   → SPI clock (1 MHz)
     * - 13        → SCK pin
     * - 12        → MISO pin
     * - 11        → MOSI pin
     */
    // if (!thermo.begin(5, SPI, 1000000, 18, 19, 23)) // ESP32
    if (!thermo.begin(10))
    {
        Serial.println("MAX31855 initialization failed!");
        while (1);
    }

    Serial.println("MAX31855 Example Started");
}

void loop()
{
    /**
     * Read 32-bit sensor frame
     *
     * Returns:
     * - true  → Valid temperature reading
     * - false → Fault detected
     */
    if (!thermo.readRaw())
    {
        Serial.print("Fault Detected: ");
        Serial.println(thermo.getFault());
    }
    else
    {
        /**
         * Print thermocouple temperature
         */
        Serial.print("Thermocouple: ");
        Serial.print(thermo.getTemperatureC());
        Serial.print(" °C | ");

        Serial.print(thermo.getTemperatureF());
        Serial.print(" °F | ");

        Serial.print(thermo.getTemperatureK());
        Serial.println(" K");

        /**
         * Print internal cold junction temperature
         */
        Serial.print("Internal Temp: ");
        Serial.print(thermo.getInternalTemperatureC());
        Serial.println(" °C");

        Serial.println("----------------------------------");
    }

    /**
     * Delay between readings
     */
    delay(1000);
}