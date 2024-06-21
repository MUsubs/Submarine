#define R2D2_DEBUG_ENABLE

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include <array>

#include "dummy_data_sender.hpp"
#include "dummy_sen_types.hpp"
#include "dummy_thermo_sensor.hpp"
#include "dummy_travel_control.hpp"
#include "r2d2_debug_macros.hpp"
#include "sub_control.hpp"

sen::DummyDataSender dummy_sender{ LED_BUILTIN, 30 };
sen::DummyThermoSensor dummy_thermo{};
sen::DummyTravelControl dummy_travel{};
sen::SubControl sub_control{ dummy_travel, dummy_sender, dummy_thermo, 1 };

std::array<uint8_t, 3> a{ 255, 127, 15 };
std::array<uint8_t, 3> b{ 1, 2, 3 };
std::array<uint8_t, 3> c{ 2, 3, 4 };
std::array<uint8_t, 3> d{ 3, 4, 5 };

namespace sen {
void simulateFullCommunication() {
    Serial.println( "==TEST== simulating full communication" );
    sub_control.receivedINST( NEW_POS, a );
    // SubControl should do nothing
    vTaskDelay(1000);

    sub_control.receivedUPDATE( CURR, b );
    sub_control.receivedINST( NEW_POS, a );
    // SubControl should set new pos and send ACK
    vTaskDelay( 1000 );

    sub_control.receivedUPDATE( CURR, b );
    sub_control.receivedUPDATE( CURR, c );
    sub_control.receivedUPDATE( CURR, d );
    // SubControl should update thrice
    vTaskDelay( 1000 );

    sub_control.receivedINST( ARRIVED );
    // SubControl should stop travel and send ACK
    // SubControl should send sensor data
    vTaskDelay( 1000 );

    // SubControl should wait for ACK
    sub_control.receivedINST( ACK );
    // SubControl should reset queues and start loop again
    vTaskDelay( 2000 );
    Serial.println( "==TEST== full communication test finished" );
    Serial.println( "=========================================" );
    Serial.println( "=========================================" );
}

void simulateStoppedCommunication() {
    Serial.println( "==TEST== simulating stopped communication" );
    sub_control.receivedINST( STOP );
    // SubControl should send ACK, reset queues and start loop again
    vTaskDelay( 1000 );

    sub_control.receivedINST( NEW_POS, a );
    sub_control.receivedINST( STOP );
    // SubControl should stop travel, send ACK, reset queues and start loop again
    vTaskDelay( 1000 );

    sub_control.receivedINST( NEW_POS, a );
    sub_control.receivedINST( ARRIVED );
    sub_control.receivedINST( STOP );
    // SubControl should send ACK, reset queues and start loop again

    vTaskDelay( 2000 );
    Serial.println( "==TEST== stopped communication test finished" );
    Serial.println( "============================================" );
    Serial.println( "============================================" );
}
}  // namespace sen

void setup() {
    pinMode( LED_BUILTIN, OUTPUT );
    Serial.begin( 9600 );
    while ( !Serial );
    vTaskDelay( 2000 );
    sub_control.activate();

    vTaskDelay( 1000 );
    sen::simulateFullCommunication();
    vTaskDelay( 10000 );
    sen::simulateStoppedCommunication();
}

void loop() {
    taskYIELD();
}
