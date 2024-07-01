#ifndef R2D2_MOTOR_HPP
#define R2D2_MOTOR_HPP

#include <Arduino.h>

namespace asn {

/**
 * @class Class Motor motor.hpp
 * @brief Controls steer, speed, and depth motors.
 */
class Motor {
public:
    /**
     * @brief Constructor for Motor class.
     *
     * @param pins (uint8_t*) Array with pins, connected to drv8833 (eep, IN1-4), and steer motor.
     */
    Motor( uint8_t *pins );

    /**
     * @brief Controls motor.
     *
     * @param motor (uint8_t array of 2) Contains both pins of a motor.
     * @param on (bool) Turns motors on if true, off if false.
     * @param dir (bool) Determines what way the motor turns.
     */
    void setMotor( uint8_t motor[2], bool on, bool dir );

    uint8_t steer[2];
    uint8_t speed[2];
    uint8_t depth[2];

private:
    uint8_t *motor_pins;
    uint8_t pin_driver_eep;
};

}  // namespace asn

#endif  // R2D2_MOTOR_HPP
