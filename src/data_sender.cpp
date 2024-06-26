#include "data_sender.hpp"

#include <semphr.h>
#include <Arduino.h>

namespace sen {

DataSender::DataSender( int led_pin, int frequency, int msg_queue_length, int task_priority ) :
    _byte_queue( xQueueCreate( msg_queue_length, sizeof(uint8_t) )  ), _unit_us( 1'000'000 / frequency ), _led_pin( led_pin )
{
    pinMode( _led_pin, OUTPUT );
    analogWriteFreq( 38000 );
    analogWriteRange( 256 );
    xTaskCreate( staticRun, "DATASENDER", 2048, this, task_priority, nullptr );
}

void DataSender::run() {
    while ( true ) {
        uint8_t byte;
        xQueueReceive( _byte_queue, &byte, portMAX_DELAY );
        
        for ( int i=0; i<8; i++ ){
            // write 38kHz signal for 1 or .5 _unit_us, depending on if bit is 1 or 0
            analogWrite( _led_pin, 127 ); 
            uint64_t wait_start = time_us_64();
            uint64_t signal_length = (byte<<i) & 0x80 ? _unit_us : _unit_us / 2;
            while ( time_us_64() < wait_start+signal_length )
                yield();
            // send low for .5 _unit_us to seperate bits
            analogWrite( _led_pin, 0 );
            wait_start = time_us_64();
            while ( time_us_64() < wait_start+_unit_us/2 )
                yield();
        }
        // wait an extra 1.5 _unit_us after byte (so total 2 unit_us wait)
        uint64_t wait_start = time_us_64();
        while ( time_us_64() < wait_start+_unit_us*1.5 )
            yield();
    }
}

void DataSender::staticRun( void * task_parameter ) {
    DataSender * self = reinterpret_cast<DataSender*>( task_parameter );
    self->run();
}

void DataSender::sendBytes( const std::vector<uint8_t>& bytes ) {
    for ( const uint8_t& byte : bytes ) {
        xQueueSend( _byte_queue, &byte, portMAX_DELAY );
    }
}

uint8_t DataSender::generateInstructionHeader( inst_t inst, uint8_t n_bytes ) {
    if ( n_bytes > 0b111 )
        return 0;
    return ( static_cast<int>( packet_t::INST ) << 6 )
        | ( static_cast<int>( inst ) << 3 )
        | n_bytes;
}

uint8_t DataSender::generateUpdateHeader( data_t data_id, uint8_t n_bytes ) {
    if ( n_bytes > 0b111 )
        return 0;
    return ( static_cast<int>( packet_t::UPDATE ) << 6 )
        | ( static_cast<int>( data_id ) << 3 )
        | n_bytes;
}

uint8_t DataSender::generateSensorHeader( sens_t sensor, uint8_t n_bytes ) {
    if ( n_bytes > 0b1111 )
        return 0;
    return ( static_cast<int>( packet_t::SENS ) << 6 )
        | ( static_cast<int>( sensor ) << 4 )
        | n_bytes;
}

} // namespace sen