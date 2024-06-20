#ifndef R2D2_DUMMY_THERMO_SENSOR_HPP
#define R2D2_DUMMY_THERMO_SENSOR_HPP

namespace sen {

class DummyThermoSensor {
public:
    DummyThermoSensor();
    float getTemperature();
};

}  // namespace sen
#endif  // R2D2_DUMMY_THERMO_SENSOR_HPP