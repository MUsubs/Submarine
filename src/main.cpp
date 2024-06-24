#include "data_receiver.hpp"
#include "data_sender.hpp"
#include "message_interpreter.hpp"
#include "photo_diode.hpp"
#include "serial_control.hpp"

#include <Arduino.h>

const int frequency = 40;

sen::DataSender sender( 22, frequency, 32, 1 );
sen::SerialControl serial_control{ sender, 1 };
sen::PhotoDiode diode( 26, frequency * 16, 20 * 4 * 2, 1 );
sen::DataReceiver receiver( frequency );
sen::MessageInterpreter interpreter( 32 );

void setup() {
    pinMode( LED_BUILTIN, OUTPUT );
    Serial.begin( 9600 );
    while ( !Serial );
    vTaskDelay( 1000 );
    diode.setListener( &receiver );
    receiver.setListener( &interpreter );
    serial_control.activate();
}

void loop() {
    if ( serial_control.getMeasurementCount() <= 0 ) {
        Serial.println( "==DEBUG== Adding measurements to buffer" );
        serial_control.addMeasure( 10 );
        serial_control.addMeasure( 20 );
        serial_control.addMeasure( 30 );
        serial_control.addMeasure( 40 );
    }
    // sender.sendBytes({0x54, 0x55, 0x56});
    delay(500);
    taskYIELD();
}