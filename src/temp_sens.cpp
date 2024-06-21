#include "temp_sens.hpp"

namespace sens {
    
    Temperature::Temperature() {}

    float Temperature::getTemp() {
        // Send the command to get temperature readings
        sensors.requestTemperatures();
        // Read and return temperature in Celsius
        return sensors.getTempCByIndex( 0 );
    }
} // namespace sens