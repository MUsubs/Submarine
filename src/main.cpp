#include "data_receiver.hpp"
#include "data_sender.hpp"
#include "message_interpreter.hpp"
#include "photo_diode.hpp"
#include "serial_control.hpp"

#define R2D2_DEBUG_ENABLE

#include <Arduino.h>
const int frequency = 30;

sen::DataSender sender( 22, frequency, 32, 2 );
sen::SerialControl serial_control{ sender, 2 };

void setup() {
    pinMode( LED_BUILTIN, OUTPUT );
    Serial.begin( 9600 );
    while ( !Serial );
    vTaskDelay( 2000 );
    serial_control.activate();
}

void loop() {
    taskYIELD();
}
