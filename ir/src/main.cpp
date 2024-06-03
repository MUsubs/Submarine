#include "ir_receiver.hpp"
#include "message_decoder.hpp"
#include "send_ir_control.hpp"

#include <unordered_set>

std::unordered_set<int> values;

class PrintListener : public sen::MessageListener {
public:
    void messageReceived( uint32_t msg ) override {
        Serial.printf( "received:\t%08x\n", msg );
        values.erase( msg );
    }
};

sen::IrReceiver * receiver;
sen::MessageDecoder * decoder;
sen::MessageListener * listener;
sen::SendIrControl * sender;

void setup() {
    // disable watchdog
    disableCore0WDT();
    disableCore1WDT();

    // serial monitor init
    Serial.begin( 57600 );
    

    // receivier objects
    receiver = new sen::IrReceiver( 26, 50 );
    decoder = new sen::MessageDecoder( *receiver, 1000 );
    listener = new PrintListener;
    decoder->setMessageListener( listener );
    // receiver task
    xTaskCreate(
        []( void* ){ receiver->main(); },
        "ir receiver",
        2048,
        NULL,
        1,
        NULL
    );

    // sender object
    sender = new sen::SendIrControl( 25, 1000, 16 );
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
    // randomSeed(2);
    // for ( int i=0; i<50; i++ )
    //     values.insert( random() );
}

void loop() { 
    // int msg = random();
    // Serial.printf("sending:\t%08x\n", msg);
    // sender->sendMessage( msg );
    sender->sendMessage( 0x5555fafa );
    delay( 2000 );
    // Serial.printf("Thingies left:\t%i\n", values.size());
}