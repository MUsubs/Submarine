#ifndef MOTOR_HPP
#define MOTOR_HPP

#include <Arduino.h>

namespace asn {

class Motor {
public:
    Motor( uint8_t *pins );
    void setMotor( uint8_t motor[2], bool on, bool dir );
    uint8_t steer[2];
    uint8_t speed[2];
    uint8_t depth[2];
    bool on = false;
    bool dir = false;

private:
    uint8_t *motor_pins;
    uint8_t pin_driver_eep;
};

}  // namespace asn

#endif  // MOTOR_HPP
