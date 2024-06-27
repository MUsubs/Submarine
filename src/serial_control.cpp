#include "serial_control.hpp"

// #define R2D2_DEBUG_ENABLE
#undef R2D2_DEBUG_ENABLE

#include "r2d2_debug_macros.hpp"
namespace sen {

// PUBLIC

SerialControl::SerialControl( DataTransceiver& data_sender, int task_priority ) :
    _data_sender( data_sender ), _measure_buffer{}, _this_task_handle{}, _state{ state_t::IDLE } {
    xTaskCreate( staticRun, "SERIAL_CONTROL", 4000, this, task_priority, &_this_task_handle );
}

void SerialControl::activate() {
    // // Serial.println( "==INFO== Activating Serial Control instance" );
    vTaskResume( _this_task_handle );
    _state = state_t::READING;
}

void SerialControl::deactiveate() {
    // // Serial.println( "==INFO== Deactivating Serial Control instance" );
    vTaskSuspend( _this_task_handle );
    _state = state_t::IDLE;
}

void SerialControl::clearMeasurements() {
    // // Serial.println( "==INFO== Clearing measurements buffer" );
    std::queue<float> empty_buffer;
    std::swap( _measure_buffer, empty_buffer );
}

void SerialControl::addMeasure( float measure ) {
    _measure_buffer.emplace( measure );
}

int SerialControl::getMeasurementCount() {
    return _measure_buffer.size();
}

void SerialControl::receivedACK() {
    _state = state_t::SERIAL_ACK;
}

// PRIVATE

void SerialControl::run( void* pvParameters ) {
    String serial_data;
    for ( ;; ) {
        // 0 = IDLE, 1 = READING, 2 = TRANSMIT, 3 = DATA_SEND
        switch ( _state ) {
            case state_t::IDLE:
                vTaskSuspend( _this_task_handle );
                break;

            case state_t::READING:
                serial_data = readSerialString();
                if ( serial_data == "" ) {
                    break;
                }
                if ( serial_data == "TRANSMIT" ) {
                    _state = state_t::SERIAL_TRANSMIT;
                    break;
                }
                _state = state_t::DATA_SEND;
                break;

            case state_t::SERIAL_TRANSMIT:
                transmitMeasures();
                _state = state_t::READING;
                break;

            case state_t::DATA_SEND:
                sendPacket( serial_data );
                _state = state_t::READING;
                break;

            case state_t::SERIAL_ACK:
                transmitACK();
                _state = state_t::READING;
                break;

            default:
                // Serial.print( "INVALID STATE" );
                // Serial.println( _state );
                break;
        }
    }
}

void SerialControl::staticRun( void* pvParameters ) {
    SerialControl* self = reinterpret_cast<SerialControl*>( pvParameters );
    self->run( pvParameters );
}

void SerialControl::transmitMeasures() {
    for ( ; !_measure_buffer.empty(); _measure_buffer.pop() ) {
        Serial.printf( "SENS,TEMP,%f\n", _measure_buffer.front() );
    }
}

void SerialControl::transmitACK() {
    Serial.printf( "INST,ACK\n" );
}

// IR Transmission
void SerialControl::sendPacket( const String& packet_string ) {
    bool b_send = false;
    std::vector<uint8_t> bytes_2_send = {};
    std::tuple<std::array<String, 10>, int> command = extractCommand( packet_string );
    std::array<String, 10>& data_arr = std::get<0>( command );
    int n_data = 0;
    for ( const auto& item : data_arr ) {
        if ( item == "" ) break;
        n_data++;
    }
    if ( std::get<1>( command ) == 0 || data_arr.empty() ) {
        Serial.printf(
            "==ERROR== invalid command '%s' with length 0 in "
            "SerialControl::sendPacket() @serial_control.cpp:%d\n",
            data_arr[0], __LINE__ );
        return;
    }
    String command_type = data_arr[0];
    if ( command_type == "INST" ) {
        if ( n_data <= 1 ) return;
        String instruction_str = data_arr[1];
        inst_t instruction_type;

        if ( _single_byte_commands.find( instruction_str ) != _single_byte_commands.end() ) {
            bytes_2_send.emplace_back( _single_byte_commands[instruction_str] );
            b_send = true;
        } else {
            if ( instruction_str == "NEW_POS" ) {
                instruction_type = inst_t::NEW_POS;
            } else {
                // Serial.printf( "==ERROR== invalid instruction '%s'\n", instruction_str.c_str() );
            }
            bytes_2_send.emplace_back( _data_sender.generateInstructionHeader(
                instruction_type, std::get<1>( command ) - 2 ) );
            b_send = true;
        }

    } else if ( command_type == "UPDATE" ) {
        String data_str = data_arr[1];
        data_t data_type;
        if ( data_str == "CURR" ) data_type = data_t::CURR;
        bytes_2_send.emplace_back(
            _data_sender.generateUpdateHeader( data_type, std::get<1>( command ) - 2 ) );
        b_send = true;

    } else {
        return;
    }

    String coord;
    for ( int i = 2; i < std::get<1>( command ); i++ ) {
        coord = data_arr[i];
        coord = coord.substring( coord.indexOf( '=' ) + 1 );
        // Serial.printf( "==DEBUG== coord substring = %s\n", coord.c_str() );
        uint8_t mapped_coord = std::min( map( coord.toFloat() * 100, 0, 100, 0, 255 ), (long)255 );
        // Serial.printf( "==DEBUG== mapped coord = %d\n", mapped_coord );
        bytes_2_send.emplace_back( mapped_coord );
    }
    // TODO: BUGFIX, ALWAYS SENDS EVEN ON INVALID INSTRUCTION
    for ( const uint8_t& b : bytes_2_send ) {
        R2D2_DEBUG_LOG( "Byte to send: %#02x", b );
    }
    if ( b_send ) _data_sender.sendBytes( bytes_2_send );
}

std::tuple<std::array<String, 10>, int> SerialControl::extractCommand( const String& input ) {
    std::array<String, 10> args;
    int from_index = 0;
    int comma_index = input.indexOf( ',', from_index );
    for ( int i = 0; i < 10; i++ ) {
        if ( comma_index == -1 ) {
            args[i] = input.substring( from_index );
            // Serial.println( "Extracted command:" );
            for ( int j = 0; j < 10; j++ ) {
                if ( args[j] == "" ) break;
                // Serial.printf( "%d : %s\n", j, args[j].c_str() );
            }
            return std::tuple<std::array<String, 10>, int>{ args, i + 1 };
        }
        args[i] = input.substring( from_index, comma_index );
        from_index = comma_index + 1;
        comma_index = input.indexOf( ',', from_index );
    }

    return std::tuple<std::array<String, 10>, int>{ args, 0 };
}

String SerialControl::readSerialString() {
    if ( Serial.available() > 0 ) {
        String result = Serial.readString();
        result.trim();
        result.replace( "\b", "" );
        return result;
    }
    return "";
}

}  // namespace sen