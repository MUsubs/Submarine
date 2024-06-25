#include <Arduino.h>
#include <FreeRTOS.h>

#include "dummyControl.hpp"
#include "motor.hpp"
#include "motor_control.hpp"
#include "mpu6050.hpp"
#include "steer_control.hpp"
#include "travel_control.hpp"

xTaskHandle motor_control_task_handle;
xTaskHandle steer_control_task_handle;
xTaskHandle travel_control_task_handle;
xTaskHandle dummy_control_task_handle;

static uint8_t motor_pins[7] = { 22, 21, 20, 19, 18, 12, 13 };
static uint8_t button_pins[4] = { 16, 17, 26, 27 };
Kalman kalmanFilter;
MPU6050 gyro( Wire );
asn::Mpu6050 mpu( gyro );
asn::MotorControl motor_control( motor_pins );
asn::SteerControl steer_control( mpu, motor_control, kalmanFilter );
asn::TravelControl travel_control( motor_control, steer_control );
asn::DummyControl dummy_control( travel_control );

void motorControlTask( void* pvParameters ) {
    asn::MotorControl* mc = reinterpret_cast<asn::MotorControl*>( pvParameters );
    mc->main();
}

void steerControlTask( void* pvParameters ) {
    asn::SteerControl* sc = reinterpret_cast<asn::SteerControl*>( pvParameters );
    sc->main();
}

void travelControlTask( void* pvParameters ) {
    asn::TravelControl* tc = reinterpret_cast<asn::TravelControl*>( pvParameters );
    tc->main();
}

void dummyControlTask( void* pvParameters ) {
    asn::DummyControl* dc = reinterpret_cast<asn::DummyControl*>( pvParameters );
    dc->main();
    vTaskDelete( dummy_control_task_handle );
}

void setup() {
    Serial.begin( 115200 );
    Wire.begin();
    steer_control.setUpSteerControl();

    vTaskDelay( 5000 );

    Serial.println( "Creating tasks..." );
    auto return_motor = xTaskCreate( motorControlTask, "MotorControl task", 2048, (void*)&motor_control, 1, &motor_control_task_handle );
    auto return_steer = xTaskCreate( steerControlTask, "SteerControl task", 2048, (void*)&steer_control, 1, &steer_control_task_handle );
    auto return_travel = xTaskCreate(travelControlTask, "TravelControl task", 2048, (void*)&travel_control, 1, &travel_control_task_handle);
    auto return_dummy = xTaskCreate( dummyControlTask, "DummyControl task", 2048, (void*)&dummy_control, 2, &dummy_control_task_handle );
}

void loop() {
    // Serial.print( "graden: " + String( round( gyro.getCurrent_z() ) ) );
    // if ( round( gyro.getCurrent_z() ) < gyro.getSetpoint() ) {
    //     Serial.println( " left " );
    //     motorControl.move( motorControl.direction_t::LEFT );
    //     delay( 50 );
    // } else if ( round( gyro.getCurrent_z() ) > gyro.getSetpoint() ) {
    //     Serial.println( " right " );
    //     motorControl.move( motorControl.direction_t::RIGHT );
    //     delay( 50 );
    // }
    // if ( digitalRead( button_pins[0] ) == HIGH ) {
    //     motorControl.move( motorControl.direction_t::FORWARD );
    //     delay( 50 );
    //     motorControl.move( motorControl.direction_t::STOP );
    // }
    // if ( digitalRead( button_pins[1] ) == HIGH ) {
    //     motorControl.move( motorControl.direction_t::BACKWARD );
    //     delay( 50 );
    //     motorControl.move( motorControl.direction_t::STOP );
    // }
    // if ( digitalRead( button_pins[2] ) == HIGH ) {
    //     motorControl.move( motorControl.direction_t::UP );
    //     delay( 50 );
    //     motorControl.move( motorControl.direction_t::STOP );
    // }
    // if ( digitalRead( button_pins[3] ) == HIGH ) {
    //     motorControl.move( motorControl.direction_t::DOWN );
    //     delay( 50 );
    //     motorControl.move( motorControl.direction_t::STOP );
    // }
}