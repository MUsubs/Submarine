#include "dummy_thermo_sensor.hpp"

namespace sen {

DummyThermoSensor::DummyThermoSensor() {
}

float DummyThermoSensor::getTemperature() {
    return 69.42f;
}

}