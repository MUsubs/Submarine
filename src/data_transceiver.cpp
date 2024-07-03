#include "data_transceiver.hpp"

#include "r2d2_debug_macros.hpp"
#include <array>

namespace sen {
// public
DataTransceiver::DataTransceiver(
    int nss_pin, int rst_pin, int dio0_pin, bool is_sub, MessageInterpreter& message_interpreter,
    int task_priority ) :
    nss_pin( nss_pin ), rst_pin( rst_pin ), dio0_pin( dio0_pin ), is_sub( is_sub ),
    _byte_queue( xQueueCreate( 10, sizeof( std::array<uint8_t, 4> ) ) ), _this_task_handle(),
    message_interpreter( message_interpreter ) {
    xTaskCreate( staticRun, "DATA_TRANSCEIVER", 5000, (void*)this, task_priority, &_this_task_handle );
}

void DataTransceiver::activate() {
    _state = state_t::COMMAND;
    if ( LoRa.begin( 868E6 ) ) {
        R2D2_DEBUG_LOG( "LoRa succesful!" );
    } else {
        R2D2_DEBUG_LOG( "LoRa unsuccesful!" );
    }
    LoRa.setPins( nss_pin, rst_pin, dio0_pin );
    vTaskResume( _this_task_handle );
}

void DataTransceiver::deactivate() {
    vTaskSuspend( _this_task_handle );
    _state = state_t::IDLE;
}

void DataTransceiver::sendBytes( std::vector<uint8_t> bytes ) {
    std::array<uint8_t, 4> send_bytes = {0, 0, 0, 0};
    for(int i = 0; i < bytes.size(); i++){
        send_bytes[i] = bytes[i];
    }
    xQueueSend( _byte_queue, (void*)&send_bytes, 0 );
}

uint8_t DataTransceiver::generateInstructionHeader( inst_t inst, uint8_t n_bytes ) {
    if ( n_bytes > 0b111 ) return 0;
    return ( static_cast<int>( packet_t::INST ) << 6 ) | ( static_cast<int>( inst ) << 3 ) | n_bytes;
}

uint8_t DataTransceiver::generateUpdateHeader( data_t data_id, uint8_t n_bytes ) {
    if ( n_bytes > 0b111 ) return 0;
    return ( static_cast<int>( packet_t::UPDATE ) << 6 ) | ( static_cast<int>( data_id ) << 3 ) | n_bytes;
}

uint8_t DataTransceiver::generateSensorHeader( sens_t sensor, uint8_t n_bytes ) {
    if ( n_bytes > 0b1111 ) return 0;
    return ( static_cast<int>( packet_t::SENS ) << 6 ) | ( static_cast<int>( sensor ) << 4 ) | n_bytes;
}

// private
void DataTransceiver::run() {
    std::array<uint8_t, 4> bytes;
    for ( ;; ) {
        switch ( _state ) {
            case state_t::IDLE:
                vTaskSuspend( _this_task_handle );
                break;

            case state_t::COMMAND:
                if ( xQueueReceive( _byte_queue, &bytes, 50 ) ) {
                    _state = state_t::SEND;
                    break;
                }
                _state = state_t::READ;
                break;

            case state_t::SEND:
                digitalWrite(LED_BUILTIN, HIGH);
                writeMessage( bytes );
                digitalWrite(LED_BUILTIN, LOW);
                _state = state_t::COMMAND;
                break;

            case state_t::READ:
                passMessages();
                _state = state_t::COMMAND;
                break;
        }
    }
}

void DataTransceiver::staticRun( void* pvParameters ) {
    DataTransceiver* data_transceiver = reinterpret_cast<DataTransceiver*>( pvParameters );
    data_transceiver->run();
    vTaskDelete( data_transceiver->_this_task_handle );
}

void DataTransceiver::passMessages() {
    int packetSize = LoRa.parsePacket();
    R2D2_DEBUG_LOG( "Received packet with %d bytes", packetSize );
    byte recipient = 0;
    if ( packetSize > 0 ) {
        recipient = LoRa.read();
    } else {
        return;
    }
    // if DataTransceiver instance is in submarine, check if destination is sub_address, else land_address
    if ( recipient != ( is_sub ? sub_address : land_address ) &&
         ( recipient == land_address || recipient == sub_address ) ) {
        // empty LoRa FIFO buffer if message isn't for us
        for ( int i = 0; i < packetSize; i++ ) {
            LoRa.read();
        }
        return;
    }
    for ( int i = 0; i < packetSize; i++ ) {
        message_interpreter.byteReceived( LoRa.read() );
    }
    message_interpreter.messageDone();
}

void DataTransceiver::writeMessage( std::array<uint8_t, 4> bytes ) {
    LoRa.beginPacket();
    // if DataTransceiver instance is in submarine, send sub_address as destination, else land_address
    LoRa.write( is_sub ? land_address : sub_address );
    for ( const auto& b : bytes ) {
        LoRa.write( b );
        R2D2_DEBUG_LOG( "Sending LoRa byte : %d", b );
    }
    LoRa.endPacket();
    digitalWrite(LED_BUILTIN, LOW);
}

}  // namespace sen
