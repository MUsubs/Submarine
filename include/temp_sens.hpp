#ifndef R2D2_TEMP_SENS_HPP
#define R2D2_TEMP_SENS_HPP

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2

namespace sens {
	
	/**
     * @brief returns a temperature in Celsius
     * @details This function returns the temperature in Celsius from a DS18B20 temperature sensor. The sensor is connected to the pin 2 defined by ONE_WIRE_BUS.
     * 
     * 
     * @param void
     * @return float Temperature in Celsius
     * @see OneWire, DallasTemperature libraries for more details.
     */
	float getTemp() {
		// Setup a oneWire instance to communicate with temperature sensor
		OneWire oneWire( ONE_WIRE_BUS );
		// Pass the oneWire reference to Dallas Temperature sensor object
		DallasTemperature sensors( &oneWire );

		// Send the command to get temperature readings
		sensors.requestTemperatures();
		// Read and return temperature in Celsius
		return sensors.getTempCByIndex( 0 );
	}

}

#endif //R2D2_TEMP_SENS_HPP