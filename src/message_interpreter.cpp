#include "message_interpreter.hpp"

namespace sen {

MessageInterpreter::MessageInterpreter( int queue_length ) :
    _packets_queue( xQueueCreate( queue_length, sizeof( uint32_t ) ) ) {
}

void MessageInterpreter::byteReceived( uint8_t msg ) {
    xQueueSend( _packets_queue, &msg, 0 );
}

void MessageInterpreter::messageDone() {
    
}

void MessageInterpreter::interpretHeader( sen::packet_t &type,
                                          sen::inst_t &instruction,
                                          sen::sens_t &sensor_id,
                                          sen::data_t &data_type,
                                          uint8_t &bytes_amount ) {
    bytes_amount = 5;
}

void MessageInterpreter::main() {
    uint8_t bytes_amount = 0;

    sen::packet_t type;
    sen::inst_t instruction;
    sen::sens_t sensor_id;
    sen::data_t data_type;

    for ( ;; ) {
        // Serial.println("CYCLE OF VLC_RECEIVER");
        switch ( state ) {
            case IDLE:

                if ( _packets_queue != NULL ) {
                    if ( uxQueueMessagesWaiting( _packets_queue ) >= 1 ) {
                        state = HEADER;
                    }
                }
                // yield();

                break;

            case HEADER:

                interpretHeader( type, instruction, sensor_id, data_type,
                                 bytes_amount );

                state = EXECUTE;

                break;

            case EXECUTE:

                // readDataPackets();

                state = ACKNOWLEDGE;

                break;

            case ACKNOWLEDGE:

                state = IDLE;

                break;
        }
    }
}

MessageInterpreter::~MessageInterpreter() {
    vQueueDelete( _packets_queue );
}

}  // namespace sen