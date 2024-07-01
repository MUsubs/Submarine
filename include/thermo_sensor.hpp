#ifndef R2D2_THERMO_SENSOR_HPP
#define R2D2_THERMO_SENSOR_HPP

#include <DallasTemperature.h>
#include <OneWire.h>

namespace sen {

class ThermoSensor {
public:
    /**
     * @brief Construct a new Temperature object
     * @details This constructor initializes the temperature sensor.
     *
     * @param one_wire_pin GPIO pin on which the OneWire bus will be constructed
     * @see OneWire, DallasTemperature libraries for more details.
     */
    ThermoSensor( int one_wire_pin );

    /**
     * @brief Get temperature in Celsius
     * @details This function returns the temperature in Celsius from a DS18B20 temperature sensor.
     *
     * @return float Temperature in Celsius
     */
    float getTemp();

private:
    OneWire oneWire;
    DallasTemperature sensor;
};

}  // namespace sen

#endif  // R2D2_THERMO_SENSOR_HPP