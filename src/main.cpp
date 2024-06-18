#include <Arduino.h>

#include "motor.hpp"
#include "motor_control.hpp"
#include "mpu6050.hpp"

// aansturing 1 main:

using namespace asn;

static uint8_t motor_pins[7] = { 22, 21, 20, 19, 18, 12, 13 };
static uint8_t button_pins[4] = { 16, 17, 26, 27 };
MotorControl motorControl( motor_pins );
MPU6050 mpu( Wire );
Kalman kalmanFilter;
Mpu6050 gyro( mpu, kalmanFilter );

void setup() {
    Wire.begin();
    gyro.setUpGyro();
    Serial.begin( 115200 );
    adc_init();
    adc_set_temp_sensor_enabled( true );
    adc_select_input( 4 );
}

void loop() {
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

// aansturing 2 main:

// #include "Wire.h"
// #include "mpu6050.hpp"

// VarSpeedServo my_servo;
// MPU6050 mpu( Wire );
// Kalman kalmanFilter;
// Mpu6050 gyro( my_servo, mpu, kalmanFilter);
// unsigned long timer = 0;

// void setup() {
//     Serial.begin( 9600 );
//     Wire.begin();
//     gyro.setUpGyro();
// }

// void loop() {

//     if ( Serial.available() > 0 ) {
//       String inputString = Serial.readStringUntil( '\n' );
//       gyro.setSetpoint( inputString.toFloat() );
//     }

//     gyro.kalman();
//     my_servo.write( gyro.PID(), 30 );
//     Serial.println(gyro.mean());

//     delay( 50 );
// }
