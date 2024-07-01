#include "motor.hpp"

namespace asn {

Motor::Motor( uint8_t* pins ) :
    motor_pins( pins ),
    // Initialize pin_driver_eep with the first element of motor_pins
    pin_driver_eep( motor_pins[0] ) {
    // Initialize arrays with the correct elements from motorpins
    steer[0] = motor_pins[5];
    steer[1] = motor_pins[6];
    speed[0] = motor_pins[1];
    speed[1] = motor_pins[2];
    depth[0] = motor_pins[3];
    depth[1] = motor_pins[4];

    // Set the pins as OUTPUT and LOW
    for ( int i = 1; i < 7; i++ ) {
        pinMode( motor_pins[i], OUTPUT );
        digitalWrite( motor_pins[i], LOW );
    }

    pinMode( pin_driver_eep, OUTPUT );
    digitalWrite( pin_driver_eep, HIGH );
}

void Motor::setMotor( uint8_t motor[2], bool on, bool dir ) {
    digitalWrite( motor[0], false );
    digitalWrite( motor[1], false );

    digitalWrite( motor[0], on && dir );
    digitalWrite( motor[1], on && !dir );
}

}  // namespace asn
