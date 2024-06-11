#include "ir_receiver.hpp"
#include "message_decoder.hpp"
#include "send_ir_control.hpp"

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <Arduino.h>

// #include <unordered_set>
// std::unordered_set<int> values;

class PrintListener : public sen::MessageListener {
public:
    void messageReceived( uint32_t msg ) override {
        // Serial.print( "received:\t" + String(msg, 2) + "\n" );
        Serial.printf( "received: %08x\n", msg );
        // values.erase( msg );
    }
};

sen::IrReceiver * receiver;
sen::MessageDecoder * decoder;
sen::MessageListener * listener;
sen::SendIrControl * sender;

void setup() {
    // serial monitor init
    Serial.begin( 57600 );
    // receivier objects
    receiver = new sen::IrReceiver( 26, 250, 15000 );
    decoder = new sen::MessageDecoder( *receiver, 2500 );
    listener = new PrintListener;
    decoder->setMessageListener( listener );
    // receiver task
    xTaskCreate(
        []( void* ){ receiver->main(); },
        "ir receiver",
        4096,
        NULL,
        1,
        NULL
    );

    // sender object
    sender = new sen::SendIrControl( 22, 2500, 16 );
    // sender task
    xTaskCreate(
        []( void* ){ sender->main(); },
        "ir sender",
        2048,
        NULL,
        1,
        NULL
    );

    // // set random seed for random()
    randomSeed(2);
    // for ( int i=0; i<50; i++ )
    //     values.insert( random() );
}

void loop() { 
    int msg = random();
    Serial.printf( "sending: %08x\n", msg );
    sender->sendMessage( msg );
    // Serial.printf("Thingies left:\t%i\n", values.size());
    delay(2123);
}