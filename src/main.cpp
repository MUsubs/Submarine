#include <Arduino.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include "data_transceiver.hpp"
#include "message_interpreter.hpp"
#include "message_passer.hpp"
#include "r2d2_debug_macros.hpp"
#include "serial_control.hpp"

sen::MessageInterpreter message_interpreter{ 20, 1 };
sen::DataTransceiver data_transceiver{ 10, 9, 2, true, message_interpreter, 1 };
sen::SerialControl serial_control{data_transceiver, 1};
sen::MessagePasser message_passer{serial_control, 1};

void setup() {
    Serial.begin( 115200 );

    while ( !Serial );
    vTaskDelay( 2000 );

    message_interpreter.setListener(&message_passer);
    
    message_interpreter.activate();
    data_transceiver.activate();
    serial_control.activate();
    message_passer.activate();
}

void loop() {
    taskYIELD();
}
