#include <Arduino.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include <array>

#include "data_transceiver.hpp"
#include "motor.hpp"
#include "motor_control.hpp"
#include "mpu6050.hpp"
#include "r2d2_debug_macros.hpp"
#include "steer_control.hpp"
#include "sub_control.hpp"
#include "thermo_sensor.hpp"
#include "travel_control.hpp"

xTaskHandle motor_control_task_handle;
xTaskHandle steer_control_task_handle;
xTaskHandle travel_control_task_handle;
xTaskHandle dummy_control_task_handle;

// EEP, speed x2, depth x2, steer x2
static uint8_t motor_pins[7] = { 18, 17, 16, 14, 15, 13, 12 };
Kalman kalman_filter;
MPU6050 gyro( Wire );
asn::Mpu6050 mpu( gyro );
asn::MotorControl motor_control( motor_pins );
asn::SteerControl steer_control( mpu, motor_control, kalman_filter );
asn::TravelControl travel_control( motor_control, steer_control );

sen::MessageInterpreter message_interpreter{ 20, 1 };
sen::DataTransceiver data_transceiver{ 2, 27, 26, true, message_interpreter, 1 };
sen::ThermoSensor thermo_sensor{ 10 };
sen::SubControl sub_control{ travel_control, data_transceiver, thermo_sensor, 1 };

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

void setup() {
    Serial.begin( 115200 );

    Wire.begin();

    Serial.println( "keep boat still" );
    steer_control.setUpSteerControl();

    delay( 100 );
    digitalWrite( LED_BUILTIN, HIGH);
    Serial.println( "bring me to the waaater nowwwww, bring me to the waaater sylvieee." ); 
    delay( 5000 );
    digitalWrite( LED_BUILTIN, LOW);


    auto return_motor = xTaskCreate(
        motorControlTask, "MotorControl task", 2048, (void*)&motor_control, 1, &motor_control_task_handle );
    auto return_steer = xTaskCreate(
        steerControlTask, "SteerControl task", 2048, (void*)&steer_control, 1, &steer_control_task_handle );
    auto return_travel = xTaskCreate(
        travelControlTask, "TravelControl task", 2048, (void*)&travel_control, 1,
        &travel_control_task_handle );

    sub_control.activate();
    message_interpreter.setListener( &sub_control );
}

void loop() {
    taskYIELD();
}
