#include "serial_control.hpp"

namespace sen {

// PUBLIC

SerialControl::SerialControl( DummyDataSender& data_sender ) :
    _data_sender( data_sender ), _measure_buffer{} {
}

void SerialControl::run() {
}

// PRIVATE

void SerialControl::addMeasure( const float& measure ) {
    _measure_buffer.emplace( measure );
}

void SerialControl::transmitMeasures() {
    for ( ; !_measure_buffer.empty(); _measure_buffer.pop() ) {
        Serial.printf( "SENS,TEMP,%f\r\t", _measure_buffer.front() );
    }
}

void SerialControl::sendPacket( const String& packet_string ) {
    std::deque<uint8_t> bytes_2_send = {};
    std::tuple<String*, int> command = extractCommand( packet_string );
    String command_type = std::get<0>( command )[0];
    if ( command_type == "INST" ) {
        String instruction_str = std::get<0>( command )[1];
        inst_t instruction_type;
        if ( instruction_str == "NEW_POS" )
            instruction_type = NEW_POS;
        else if ( instruction_str == "ARRIVED" )
            instruction_type = ARRIVED;
        else if ( instruction_str == "STOP" )
            instruction_type = STOP;
        else if ( instruction_str == "ACK" )
            instruction_type = ACK;
        bytes_2_send.emplace_back( _data_sender.generateInstructionHeader(
            instruction_type, std::get<1>( command ) - 2 ) );

    } else if ( command_type == "UPDATE" ) {
        String data_str = std::get<0>( command )[1];
        data_t data_type;
        if ( data_str == "CURR" ) data_type = CURR;
        bytes_2_send.emplace_back( _data_sender.generateUpdateHeader(
            data_type, std::get<1>( command ) - 2 ) );

    } else if ( command_type == "TRANSMIT" ) {
        transmitMeasures();
        return;

    } else {
        return;
    }

    String coord;
    for ( int i = 2; i < std::get<1>( command ); i++ ) {
        coord = std::get<0>( command )[i];
        coord = coord.substring(coord.indexOf('=')+1);
        bytes_2_send.emplace_back( map( coord.toFloat(), 0.0f, 1.0f, 0, 255 ) );
    }
    _data_sender.sendBytes( bytes_2_send );
}

std::tuple<String*, int> SerialControl::extractCommand( const String& input ) {
    String args[10];
    int from_index = 0;
    int comma_index = input.indexOf( ',', from_index );
    for ( int i = 0; i < 10; i++ ) {
        if ( comma_index == -1 ) return std::tuple<String*, int>{ args, i + 1 };
        args[i] = input.substring( from_index, comma_index );
        from_index = comma_index + 1;
        comma_index = input.indexOf( ',', from_index );
    }
}

String SerialControl::readSerial() {
    if (Serial.available() > 0){
        return Serial.readString();
    }
}

}  // namespace sen