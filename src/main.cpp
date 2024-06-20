#include <FreeRTOS.h>
#include <task.h>

#include "dummy_data_sender.hpp"
#include "dummy_travel_control.hpp"
#include "sub_control.hpp"

sen::DummyDataSender dummy_sender{ LED_BUILTIN, 30 };

void setup() {
    pinMode( LED_BUILTIN, OUTPUT );
    Serial.begin( 9600 );
}

void loop() {
    taskYIELD();
}
