#include "data_receiver.hpp"
#include "data_sender.hpp"
#include "message_interpreter.hpp"
#include "photo_diode.hpp"
#include "serial_control.hpp"

#define R2D2_DEBUG_ENABLE

#include <Arduino.h>
const int frequency = 30;

sen::DataSender sender( 22, frequency, 32, 1 );
sen::SerialControl serial_control{ sender, 1 };

void setup() {
    pinMode( LED_BUILTIN, OUTPUT );
    pinMode( 7, INPUT );
    pinMode( 8, INPUT );

    Serial.begin( 9600 );
    serial_control.activate();
}

bool b_do = false;

void loop() {
    if ( serial_control.getMeasurementCount() ) {
        digitalWrite( LED_BUILTIN, HIGH );
    } else {
        digitalWrite( LED_BUILTIN, LOW );
    }
    if ( !digitalRead( 7 ) && !digitalRead( 8 ) ) {
        b_do = true;
    }
    if ( digitalRead( 7 ) && b_do ) {
        serial_control.receivedACK();
        b_do = false;
    }
    if ( digitalRead( 8 ) && b_do ) {
        serial_control.addMeasure( analogReadTemp() );
        b_do = false;
    }
    taskYIELD();
}