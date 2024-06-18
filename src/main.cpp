#include <Arduino.h>

#include "motor.hpp"
#include "motor_control.hpp"
#include "mpu6050.hpp"

using namespace asn;

static uint8_t motor_pins[7] = { 22, 21, 20, 19, 18, 12, 13 };
static uint8_t button_pins[4] = { 16, 17, 26, 27 };
MotorControl motorControl( motor_pins );
MPU6050 mpu( Wire );
Kalman kalmanFilter;
Mpu6050 gyro( mpu, kalmanFilter );

void setup() {
    Wire.begin();
    gyro.setGyroUp();
    Serial.begin( 115200 );
    adc_init();
    adc_set_temp_sensor_enabled( true );
    adc_select_input( 4 );
}

void loop() {
    Serial.print( "graden: " + String( round( gyro.getCurrent_z() ) ) );
    if ( round( gyro.getCurrent_z() ) < gyro.getSetpoint() ) {
        Serial.println( " left " );
        motorControl.move( motorControl.direction_t::LEFT );
        delay( 50 );
    } else if ( round( gyro.getCurrent_z() ) > gyro.getSetpoint() ) {
        Serial.println( " right " );
        motorControl.move( motorControl.direction_t::RIGHT );
        delay( 50 );
    }
    if ( digitalRead( button_pins[0] ) == HIGH ) {
        motorControl.move( motorControl.direction_t::FORWARD );
        delay( 50 );
        motorControl.move( motorControl.direction_t::STOP );
    }
    if ( digitalRead( button_pins[1] ) == HIGH ) {
        motorControl.move( motorControl.direction_t::BACKWARD );
        delay( 50 );
        motorControl.move( motorControl.direction_t::STOP );
    }
    if ( digitalRead( button_pins[2] ) == HIGH ) {
        motorControl.move( motorControl.direction_t::UP );
        delay( 50 );
        motorControl.move( motorControl.direction_t::STOP );
    }
    if ( digitalRead( button_pins[3] ) == HIGH ) {
        motorControl.move( motorControl.direction_t::DOWN );
        delay( 50 );
        motorControl.move( motorControl.direction_t::STOP );
    }
}
