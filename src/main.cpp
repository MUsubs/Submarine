#include "dummy_data_sender.hpp"
#include "serial_control.hpp"

sen::DummyDataSender dummy_sender{ LED_BUILTIN, 30 };
sen::SerialControl serial_control{ dummy_sender, 1 };

void setup() {
    pinMode( LED_BUILTIN, OUTPUT );
    Serial.begin( 9600 );
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
    taskYIELD();
}