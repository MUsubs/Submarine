#include <Arduino.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include "data_transceiver.hpp"
#include "message_interpreter.hpp"
#include "r2d2_debug_macros.hpp"
#include "message_printer.hpp"

bool is_sub = true;

sen::MessageInterpreter message_interpreter{ 20, 1 };
sen::DataTransceiver data_transceiver{ 10, 9, 2, is_sub, message_interpreter, 1 };
sen::MessagePrinter message_printer{};

void setup() {
    Serial.begin( 115200 );

    while ( !Serial );
    vTaskDelay( 2000 );
    
    message_interpreter.setListener(&message_printer);

    message_interpreter.activate();
    data_transceiver.activate();
}

void loop() {
    taskYIELD();
}
