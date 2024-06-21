#ifndef R2D2_TEMP_SENS_HPP
#define R2D2_TEMP_SENS_HPP

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2

namespace sens {
    
    class Temperature {
    public:
		/**
		 * @brief Construct a new Temperature object
		 * @details This constructor initializes the temperature sensor.
		 * 
		 * @param void
		 * @return Temperature object
		 * @see OneWire, DallasTemperature libraries for more details.
		 */
		Temperature();

        /**
         * @brief returns a temperature in Celsius
         * @details This function returns the temperature in Celsius from a DS18B20 temperature sensor. The sensor is connected to the pin 2 defined by ONE_WIRE_BUS.
         * 
         * @param void
         * @return float Temperature in Celsius
         * @see OneWire, DallasTemperature libraries for more details.
         */
        float getTemp();

	private:
		OneWire oneWire(ONE_WIRE_BUS);
		DallasTemperature sensors(&oneWire);
    };

}

#endif //R2D2_TEMP_SENS_HPP