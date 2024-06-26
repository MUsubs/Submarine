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
#include "travel_control.hpp"
#include "thermo_sensor.hpp"

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

sen::MessageInterpreter message_interpreter{ 20, 1 };
sen::DataTransceiver data_transceiver{ 10, 9, 2, true, message_interpreter, 1 };
sen::ThermoSensor thermo_sensor{21};
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

namespace sen {
std::array<uint8_t, 3> a{ 255, 127, 15 };
std::array<uint8_t, 3> b{ 1, 2, 3 };
std::array<uint8_t, 3> c{ 2, 3, 4 };
std::array<uint8_t, 3> d{ 3, 4, 5 };

void simulateFullCommunication() {
    Serial.println( "==TEST== simulating full communication" );
    sub_control.receivedINST( inst_t::NEW_POS, a );
    // SubControl should do nothing
    vTaskDelay( 1000 );

    sub_control.receivedUPDATE( data_t::CURR, b );
    // TravelControl should update
    vTaskDelay( 1000 );
    sub_control.receivedINST( inst_t::NEW_POS, a );
    // SubControl should set new pos and send ACK
    // TravelControl should set new pos
    vTaskDelay( 1000 );

    sub_control.receivedUPDATE( data_t::CURR, b );
    vTaskDelay( 500 );
    sub_control.receivedUPDATE( data_t::CURR, c );
    vTaskDelay( 500 );
    sub_control.receivedUPDATE( data_t::CURR, d );
    // SubControl should update thrice
    // Travelcontrol should update thrice
    vTaskDelay( 1000 );

    sub_control.receivedINST( inst_t::ARRIVED );
    // SubControl should stop travel and send ACK
    // TravelControl should stop travel
    // SubControl should send sensor data
    vTaskDelay( 1000 );

    // SubControl should wait for ACK
    sub_control.receivedINST( inst_t::ACK );
    // SubControl should reset queues and start loop again
    vTaskDelay( 2000 );
    Serial.println( "==TEST== full communication test finished" );
    Serial.println( "=========================================" );
    Serial.println( "=========================================" );
}

void simulateStoppedCommunication() {
    Serial.println( "==TEST== simulating stopped communication" );
    sub_control.receivedINST( inst_t::STOP );
    // SubControl should send ACK, reset queues and start loop again
    vTaskDelay( 1000 );

    sub_control.receivedUPDATE( data_t::CURR, b );
    vTaskDelay( 500 );
    sub_control.receivedINST( inst_t::NEW_POS, a );
    sub_control.receivedINST( inst_t::STOP );
    // SubControl should stop travel, send ACK, reset queues and start loop again
    // TravelControl should stop travel
    vTaskDelay( 1000 );

    sub_control.receivedUPDATE( data_t::CURR, b );
    vTaskDelay( 500 );
    sub_control.receivedINST( inst_t::NEW_POS, a );
    vTaskDelay( 2000 );
    sub_control.receivedINST( inst_t::ARRIVED );
    vTaskDelay( 2000 );
    sub_control.receivedINST( inst_t::STOP );
    // SubControl should send ACK, reset queues and start loop again

    vTaskDelay( 2000 );
    Serial.println( "==TEST== stopped communication test finished" );
    Serial.println( "============================================" );
    Serial.println( "============================================" );
}
}  // namespace sen

void setup() {
    Serial.begin( 115200 );

    while ( !Serial );
    vTaskDelay( 2000 );

    Wire.begin();
    steer_control.setUpSteerControl();

    vTaskDelay( 5000 );

    Serial.println( "Creating tasks..." );
    auto return_motor = xTaskCreate(
        motorControlTask, "MotorControl task", 2048, (void*)&motor_control, 1, &motor_control_task_handle );
    auto return_steer = xTaskCreate(
        steerControlTask, "SteerControl task", 2048, (void*)&steer_control, 1, &steer_control_task_handle );
    auto return_travel = xTaskCreate(
        travelControlTask, "TravelControl task", 2048, (void*)&travel_control, 1,
        &travel_control_task_handle );

    sub_control.activate();
    message_interpreter.setListener(&sub_control);

    vTaskDelay( 1000 );
    sen::simulateFullCommunication();
    vTaskDelay( 10000 );
    sen::simulateStoppedCommunication();
}

void loop() {
    taskYIELD();
}
