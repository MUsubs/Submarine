#include "dummy_data_sender.hpp"

namespace sen {

DummyDataSender::DummyDataSender( int led_pin, int freq ) :
    _led_pin( led_pin ), _freq( freq ) {
}

void DummyDataSender::sendBytes( const std::deque<uint8_t>& bytes ) {
    for ( const uint8_t& byte : bytes ) {
        Serial.printf( "Sending byte %d\n", byte );
    }
}

uint8_t DummyDataSender::generateInstructionHeader( inst_t inst,
                                                    uint8_t n_bytes ) {
    if ( n_bytes > 0b111 ) return 0;
    return ( INST << 6 ) | ( inst << 3 ) | n_bytes;
}

uint8_t DummyDataSender::generateUpdateHeader( data_t data_id,
                                               uint8_t n_bytes ) {
    if ( n_bytes > 0b111 ) return 0;
    return ( UPDATE << 6 ) | ( data_id << 3 ) | n_bytes;
}

uint8_t DummyDataSender::generateSensorHeader( sens_t sensor,
                                               uint8_t n_bytes ) {
    if ( n_bytes > 0b1111 ) return 0;
    return ( SENS << 6 ) | ( sensor << 4 ) | n_bytes;
}

}  // namespace sen