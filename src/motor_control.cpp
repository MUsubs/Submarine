#include "motor_control.hpp"

namespace asn {

MotorControl::MotorControl( uint8_t *pins ) : motor( pins ) {
}

void MotorControl::move( direction_t direction ) {
    switch ( direction ) {
        case LEFT:
            motor.setMotor( motor.steer, true, true );
            break;
        case RIGHT:
            motor.setMotor( motor.steer, true, false );
            break;
        case FORWARD:
            motor.setMotor( motor.speed, true, true );
            break;
        case BACKWARD:
            motor.setMotor( motor.speed, true, false );
            break;
        case UP:
            motor.setMotor( motor.depth, true, true );
            break;
        case DOWN:
            motor.setMotor( motor.depth, true, false );
            break;
        case STOP:
            motor.setMotor( motor.steer, false, false );
            motor.setMotor( motor.speed, false, false );
            motor.setMotor( motor.depth, false, false );
            break;
    }
}

}  // namespace asn