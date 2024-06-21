#include "photo_diode.hpp"
#include "data_receiver.hpp"
#include "message_interpreter.hpp"
#include "data_sender.hpp"

#include <unordered_set>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <Arduino.h>

#define FREQUENCY 400

sen::PhotoDiode diode( 26, FREQUENCY * 20, 20 * 4 * 2 );
sen::DataReceiver receiver( FREQUENCY );
sen::MessageInterpreter interpreter( 32 );
sen::DataSender sender( 22, FREQUENCY, 32, 1 );

void setup() {
    // serial monitor init
    Serial.begin( 57600 );

    // receivier task
    diode.setListener( &receiver );
    receiver.setListener( &interpreter );

    xTaskCreate(
        []( void* ){ diode.main(); },
        "ir receiver",
        4096,
        NULL,
        1,
        NULL
    );

    // make random pseudo
    randomSeed(2);
}

void loop() {
    uint8_t msg = random();
    Serial.printf( "sending %02x\n", msg );
    sender.sendBytes( { msg } );
    delay( 1000 );
}