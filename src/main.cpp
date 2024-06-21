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
sen::PhotoDiode diode( 26, frequency * 20, 20 * 4 * 2, 2 );
sen::DataReceiver receiver( frequency );
sen::MessageInterpreter interpreter( 32 );

void setup() {
    pinMode( LED_BUILTIN, OUTPUT );
    Serial.begin( 9600 );
    while ( !Serial );
    vTaskDelay( 2000 );
    serial_control.activate();
    diode.setListener( &receiver );
    receiver.setListener( &interpreter );
}

void loop() {
    if ( serial_control.getMeasurementCount() <= 0 ) {
        Serial.println( "==DEBUG== Adding measurements to buffer" );
        serial_control.addMeasure( 10 );
        serial_control.addMeasure( 20 );
        serial_control.addMeasure( 30 );
        serial_control.addMeasure( 40 );
    }
    // sender.sendBytes({5, 6, 7});
    // vTaskDelay(2000);
    taskYIELD();
}