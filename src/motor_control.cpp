#include "motor_control.hpp"

namespace asn {

MotorControl::MotorControl( uint8_t *pins ) : motor( pins ) {
    directions_queue = xQueueCreate( 10, sizeof( direction_t ) );
}

void MotorControl::move( direction_t direction ) {
    xQueueSend( directions_queue, (void *)&direction, 0 );
    // Serial.println( "received direction" );
}

void MotorControl::main() {
    Serial.println( "start motor" );
    direction_t direction;
    for ( ;; ) {
        if ( xQueueReceive( directions_queue, (void *)&direction, 0 ) ) {
            switch ( direction ) {
                case LEFT:
                    // Serial.println( "links" );
                    motor.setMotor( motor.steer, true, true );
                    break;
                case RIGHT:
                    // Serial.println( "rechts" );
                    motor.setMotor( motor.steer, true, false );
                    break;
                case FORWARD:
                    // Serial.println( "voren" );
                    motor.setMotor( motor.speed, true, true );
                    break;
                case BACKWARD:
                    // Serial.println( "achter" );
                    motor.setMotor( motor.speed, true, false );
                    break;
                case UP:
                    // Serial.println("boven");
                    motor.setMotor( motor.depth, true, true );
                    break;
                case DOWN:
                    // Serial.println( "beneden");
                    motor.setMotor( motor.depth, true, false );
                    break;
                case STOP:
                    // Serial.println( "STOP" );
                    motor.setMotor( motor.steer, false, false );
                    motor.setMotor( motor.speed, false, false );
                    motor.setMotor( motor.depth, false, false );
                    break;
                default:
                    // Serial.println("aaaaaaaa");
                    break;
            }
        }
    }
}
};  // namespace asn