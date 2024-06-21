#include "photo_diode.hpp"
#include "data_receiver.hpp"
#include "message_interpreter.hpp"
#include "data_sender.hpp"

#include <unordered_set>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <Arduino.h>

#define FREQUENCY 1500

template<typename T>
class SharedSet {
public:
    SharedSet( ) : _mutex( xSemaphoreCreateMutex( ) ) { }
    void add( T value ) {
        xSemaphoreTake( _mutex, portMAX_DELAY );
        _set.insert( value );
        xSemaphoreGive( _mutex );
    }
    bool contains( T value ) {
        xSemaphoreTake( _mutex, portMAX_DELAY );
        bool r = _set.find( value ) != _set.end();
        xSemaphoreGive( _mutex );
        return r;
    }
    void remove( T value ) {
        xSemaphoreTake( _mutex, portMAX_DELAY );
        _set.erase( value );
        xSemaphoreGive( _mutex );
    }
    std::unordered_set<T> copy_set( ) {
        xSemaphoreTake( _mutex, portMAX_DELAY );
        std::unordered_set<T> r = _set;
        xSemaphoreGive( _mutex );
        return r;
    }
    ~SharedSet() {
        vSemaphoreDelete( _mutex );
    }
private:
    std::unordered_set<T> _set;
    SemaphoreHandle_t _mutex;
};

SharedSet<uint8_t> start_values;
SharedSet<uint8_t> send_values;
SharedSet<uint8_t> received_values;

sen::PhotoDiode receiver( 26, FREQUENCY/10, FREQUENCY*4*2 );
sen::DataReceiver decoder( FREQUENCY );
sen::MessageInterpreter listener( 32 );
sen::DataSender sender( 22, FREQUENCY, 32, 1 );

void setup() {
    // serial monitor init
    Serial.begin( 57600 );

    // receivier task
    decoder.setListener( &listener );
    xTaskCreate(
        []( void* ){ receiver.main(); },
        "ir receiver",
        4096,
        NULL,
        1,
        NULL
    );

    // add some random values to test_values
    randomSeed(2);
    for ( int i = 0; i < 50; i++ )
        start_values.add( random() );
    for ( auto i : start_values.copy_set() )
        send_values.add( i );
}

void loop() {
    auto set = send_values.copy_set();
    if ( !set.empty() ) {
        auto msg = *set.begin();
        send_values.remove( msg );

        Serial.printf( "sending: %08x\n", msg );
        sender.sendBytes( { msg } );

        int correct_msg_count = 0;
        for ( auto i : start_values.copy_set() ) {
            if ( received_values.contains( i ) ) {
                correct_msg_count++;
            }
        }

        Serial.printf( "Thingies received:\t%i\n", correct_msg_count );

        delay( 2000 );
    }
}