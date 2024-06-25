#include "thermo_sensor.hpp"

namespace sen {

// public
ThermoSensor::ThermoSensor( int one_wire_pin ) : oneWire( one_wire_pin ), sensor( &oneWire ) {
}

float ThermoSensor::getTemp() {
    // Send the command to get temperature readings
    sensor.requestTemperatures();
    // Read and return temperature in Celsius
    return sensor.getTempCByIndex( 0 );
}
}  // namespace sen