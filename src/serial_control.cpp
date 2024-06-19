#include "serial_control.hpp"

namespace sen {

// PUBLIC

SerialControl::SerialControl( DummyDataSender& data_sender,
                              int task_priority ) :
    _data_sender( data_sender ), _measure_buffer{}, _this_task_handle{},
    state{} {
    xTaskCreate( staticRun, "SERIAL_CONTROL", 4000, this, task_priority,
                 &_this_task_handle );
}

void SerialControl::activate() {
    Serial.println( "==INFO== Activating Serial Control instance" );
    vTaskResume( _this_task_handle );
    state = state_t::READING;
}

void SerialControl::deactiveate() {
    Serial.println( "==INFO== Deactivating Serial Control instance" );
    vTaskSuspend( _this_task_handle );
    state = state_t::IDLE;
}

void SerialControl::clearMeasurements() {
    Serial.println( "==INFO== Clearing measurements buffer" );
    std::queue<float> empty_buffer;
    std::swap( _measure_buffer, empty_buffer );
}

void SerialControl::addMeasure( const float& measure ) {
    _measure_buffer.emplace( measure );
}

int SerialControl::getMeasurementCount() {
    return _measure_buffer.size();
}

// PRIVATE

void SerialControl::run( void* pvParameters ) {
    String serial_data;
    for ( ;; ) {
        // 0 = IDLE, 1 = READING, 2 = TRANSMIT, 3 = DATA_SEND
        switch ( state ) {
            case state_t::IDLE:
                vTaskSuspend( _this_task_handle );
                break;

            case state_t::READING:
                serial_data = readSerialString();
                if ( serial_data == "" ) {
                    break;
                }
                if ( serial_data == "TRANSMIT" ) {
                    state = state_t::SERIAL_TRANSMIT;
                    break;
                }
                state = state_t::DATA_SEND;
                break;

            case state_t::SERIAL_TRANSMIT:
                transmitMeasures();
                state = state_t::READING;
                break;

            case state_t::DATA_SEND:
                sendPacket( serial_data );
                state = state_t::READING;
                break;

            default:
                Serial.print( "INVALID STATE" );
                Serial.println( state );
                break;
        }
    }
}

void SerialControl::staticRun( void* pvParameters ) {
    SerialControl* self = reinterpret_cast<SerialControl*>( pvParameters );
    self->run( pvParameters );
}

void SerialControl::transmitMeasures() {
    Serial.printf( "==INFO== Transmitting, measure_buffer has %d items\n",
                   _measure_buffer.size() );
    for ( ; !_measure_buffer.empty(); _measure_buffer.pop() ) {
        Serial.printf( "SENS,TEMP,%f\n", _measure_buffer.front() );
    }
}

void SerialControl::sendPacket( const String& packet_string ) {
    std::deque<uint8_t> bytes_2_send = {};
    std::tuple<std::array<String, 10>, int> command =
        extractCommand( packet_string );
    if ( std::get<1>( command ) == 0 || std::get<0>( command ).empty() ) {
        Serial.printf(
            "==ERROR== invalid command '%s' with length 0 in "
            "SerialControl::sendPacket() @serial_control.cpp:%d\n",
            std::get<0>( command )[0], __LINE__ );
        return;
    }
    String command_type = std::get<0>( command )[0];
    if ( command_type == "INST" ) {
        Serial.println( "==INFO== Command is Instruction" );
        String instruction_str = std::get<0>( command )[1];
        inst_t instruction_type;

        if ( instruction_str == "NEW_POS" ) {
            instruction_type = NEW_POS;
            Serial.println( "Instruction = NEW_POS" );

        } else if ( instruction_str == "ARRIVED" ) {
            instruction_type = ARRIVED;
            Serial.println( "Instruction = ARRIVED" );

        } else if ( instruction_str == "STOP" ) {
            instruction_type = STOP;
            Serial.println( "Instruction = STOP" );

        } else if ( instruction_str == "ACK" ) {
            instruction_type = ACK;
            Serial.println( "Instruction = ACK" );

        } else {
            Serial.printf( "==ERROR== invalid instruction '%s'\n",
                           instruction_str.c_str() );
        }

        bytes_2_send.emplace_back( _data_sender.generateInstructionHeader(
            instruction_type, std::get<1>( command ) - 2 ) );

    } else if ( command_type == "UPDATE" ) {
        String data_str = std::get<0>( command )[1];
        data_t data_type;
        if ( data_str == "CURR" ) data_type = CURR;
        bytes_2_send.emplace_back( _data_sender.generateUpdateHeader(
            data_type, std::get<1>( command ) - 2 ) );

    } else {
        return;
    }

    String coord;
    for ( int i = 2; i < std::get<1>( command ); i++ ) {
        coord = std::get<0>( command )[i];
        coord = coord.substring( coord.indexOf( '=' ) + 1 );
        Serial.printf( "==DEBUG== coord substring = %s\n", coord.c_str() );
        Serial.printf( "==DEBUG== mapped coord = %d\n",
                       map( coord.toFloat() * 100, 0, 100, 0, 255 ) );
        bytes_2_send.emplace_back(
            map( coord.toFloat() * 100, 0, 100, 0, 255 ) );
    }
    _data_sender.sendBytes( bytes_2_send );
}

std::tuple<std::array<String, 10>, int> SerialControl::extractCommand(
    const String& input ) {
    std::array<String, 10> args;
    int from_index = 0;
    int comma_index = input.indexOf( ',', from_index );
    for ( int i = 0; i < 10; i++ ) {
        if ( comma_index == -1 ) {
            args[i] = input.substring( from_index );
            Serial.println( "Extracted command:" );
            for ( int j = 0; j < 10; j++ ) {
                if ( args[j] == "" ) break;
                Serial.printf( "%d : %s\n", j, args[j].c_str() );
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
    digitalWrite( LED_BUILTIN, HIGH );
    if ( Serial.available() > 0 ) {
        digitalWrite( LED_BUILTIN, LOW );
        String result = Serial.readString();
        Serial.printf( "INCOMING : %s\n", result.c_str() );
        result.trim();
        return result;
    }
    return "";
}

}  // namespace sen