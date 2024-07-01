#include <Arduino.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include "data_transceiver.hpp"
#include "message_interpreter.hpp"
#include "message_passer.hpp"
#include "packet_enums.hpp"

bool is_sub = false;

sen::MessageInterpreter message_interpreter{ 20, 1 };
sen::DataTransceiver data_transceiver{ 10, 9, 2, is_sub, message_interpreter, 1 };
sen::SerialControl serial_control{ data_transceiver, 1 };
sen::MessagePasser message_passer{ serial_control, 1 };

void setup() {
    Serial.begin( 115200 );

    message_interpreter.setListener( &message_passer );

    message_interpreter.activate();
    data_transceiver.activate();
    serial_control.activate();
    message_passer.activate();
}

void loop() {
    taskYIELD();
}
