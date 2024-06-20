#include "message_interpreter.hpp"

namespace sen {

MessageInterpreter::MessageInterpreter( int queue_length ) :
    _packets_queue( xQueueCreate( queue_length, sizeof( uint32_t ) ) ),
    _message_done_queue( xQueueCreate( 10, sizeof( uint8_t ) ) ) {
}

void MessageInterpreter::messageReceived( uint8_t msg ) {
    xQueueSend( _packets_queue, &msg, 0 );
}

void MessageInterpreter::messageDone() {
    uint8_t msg = 1;
    xQueueSend( _message_done_queue, &msg, 0 );
}

void MessageInterpreter::interpretHeader( sen::packet_t &type,
                                          sen::inst_t &instruction,
                                          sen::sens_t &sensor_id,
                                          sen::data_t &data_type,
                                          uint8_t &bytes_amount ) {
    uint8_t header = 0;
    if(!xQueueReceive( _packets_queue, &header, 0 )){
        //error handling?
    }
    bytes_amount = 5;
}

void MessageInterpreter::main() {
    uint8_t bytes_amount = 0;

    sen::packet_t type;
    sen::inst_t instruction;
    sen::sens_t sensor_id;
    sen::data_t data_type;

    uint8_t useless_byte;

    for ( ;; ) {
        // Serial.println("CYCLE OF VLC_RECEIVER");
        switch ( state ) {
            case IDLE:

                if ( _message_done_queue != NULL && _packets_queue != NULL ) {
                    if ( uxQueueMessagesWaiting( _message_done_queue ) >= 1 ) {
                        xQueueReceive( _message_done_queue, &useless_byte, 0 );
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