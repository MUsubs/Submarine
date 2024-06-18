#include "serial_control.hpp"

namespace sen {

// PUBLIC

SerialControl::SerialControl( DummyDataSender& data_sender ) :
    _data_sender( data_sender ), _measure_buffer{} {
}

void SerialControl::run() {
}

// PRIVATE

void SerialControl::addMeasure( const uint32_t& measure ) {
    _measure_buffer.emplace( measure );
}

void SerialControl::transmitMeasures() {
    for ( ; !_measure_buffer.empty(); _measure_buffer.pop() ) {
        Serial.printf( "SENS,TEMP,%u\r\t", _measure_buffer.front() );
    }
}

void SerialControl::sendPacket( const String& packet_string ) {
    std::deque<uint8_t> bytes_2_send = {};
    int from_index = 0;
    int comma_index = packet_string.indexOf( ',', from_index );
    while ( comma_index != -1 ) {
        bytes_2_send.emplace_back( (uint8_t)( String( packet_string.substring(
                                                  from_index, comma_index ) ) )
                                       .toInt() );
        comma_index = packet_string.indexOf( ',', from_index );
    }
    _data_sender.sendBytes( bytes_2_send );
}

String SerialControl::readSerial() {
    while ( Serial.available() > 0 ) {
    }
    return String();
}

std::tuple<String*, int> SerialControl::extractCommand( String& input ) {
    String args[10];
    int from_index = 0;
    int comma_index = input.indexOf( ',', from_index );
    for (int i = 0; i < 10; i++){
        if (comma_index == -1) return std::tuple<String*, int>{args, i+1};
        args[i] = input.substring(from_index, comma_index);
        from_index = comma_index + 1;
        comma_index = input.indexOf(',', from_index);
    }
}

}  // namespace sen