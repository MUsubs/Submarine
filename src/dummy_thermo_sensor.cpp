#include "dummy_thermo_sensor.hpp"

namespace sen {

DummyThermoSensor::DummyThermoSensor() {
}

float DummyThermoSensor::getTemperature() {
    return 10.0f;
}

}