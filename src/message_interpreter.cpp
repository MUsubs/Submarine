#include "message_interpreter.hpp"

#include "message_interpreter_listener.hpp"

namespace sen {

MessageInterpreter::MessageInterpreter( int queue_length, int task_priority ) :
    _packets_queue( xQueueCreate( queue_length, sizeof( uint8_t ) ) ),
    _message_done_queue( xQueueCreate( 10, sizeof( uint8_t ) ) ), _this_task_handle{}, data_array{},
    useless_byte{}, listener{} {
    xTaskCreate( staticRun, "MESSAGE_INTERPRETER", 4000, (void *)this, task_priority, &_this_task_handle );
}

void MessageInterpreter::activate() {
    _state = state_t::READ;
    vTaskResume( _this_task_handle );
}

void MessageInterpreter::deactivate() {
    vTaskSuspend( _this_task_handle );
    _state = state_t::IDLE;
}

void MessageInterpreter::byteReceived( uint8_t msg ) {
    xQueueSend( _packets_queue, &msg, 0 );
}

void MessageInterpreter::messageDone() {
    uint8_t msg = 1;
    xQueueSend( _message_done_queue, &msg, 0 );
}

void MessageInterpreter::setListener( MessageInterpreterListener *set_listener ) {
    listener = set_listener;
}

void MessageInterpreter::interpretHeader(
    sen::packet_t &type, sen::inst_t &instruction, sen::sens_t &sensor_id, sen::data_t &data_type,
    uint8_t &bytes_amount ) {
    uint8_t header = 0;

    if ( !xQueueReceive( _packets_queue, &header, 0 ) ) {
        return;
    }

    // Extract and assign the type
    type = static_cast<sen::packet_t>( ( header & 0b11000000 ) >> 6 );

    if ( type == sen::packet_t::SENS ) {
        // Extract and assign the instruction
        sensor_id = static_cast<sen::sens_t>( ( header & 0b00110000 ) >> 4 );

        // Extract and assign the bytes amount
        bytes_amount = header & 0b00001111;

        readDataPackets( bytes_amount );

    } else if ( type == sen::packet_t::INST ) {
        // Extract and assign the instruction
        instruction = static_cast<sen::inst_t>( ( header & 0b00111000 ) >> 3 );

        // Extract and assign the bytes amount
        bytes_amount = header & 0b00000111;

        if ( instruction == sen::inst_t::NEW_POS && bytes_amount > 0 ) {
            readDataPackets( bytes_amount );
        }

    } else if ( type == sen::packet_t::UPDATE ) {
        // Extract and assign the instruction
        data_type = static_cast<sen::data_t>( ( header & 0b00111000 ) >> 3 );

        // Extract and assign the bytes amount
        bytes_amount = header & 0b00000111;

        readDataPackets( bytes_amount );
    }
}

void MessageInterpreter::readDataPackets( uint8_t &bytes_amount ) {
    if ( bytes_amount <= 0 ) {
        return;
    }
    uint8_t byte = 0;

    if ( bytes_amount < DATA_ARRAY_SIZE ) {
        for ( unsigned int i = 0; i < bytes_amount; i++ ) {
            xQueueReceive( _packets_queue, &byte, 0 );
            data_array[i] = byte;
        }
    }
}

void MessageInterpreter::clear() {
    bytes_amount = 0;
    type = packet_t::NONE;
    instruction = inst_t::NONE;
    sensor_id = sens_t::NONE;
    data_type = data_t::NONE;
    uint8_t useless_byte = 0;
    data_array.fill( 0 );
}

void MessageInterpreter::run() {
    for ( ;; ) {
        switch ( _state ) {
            case IDLE:
                vTaskSuspend( _this_task_handle );
                break;

            case READ:
                if ( _message_done_queue != NULL && _packets_queue != NULL ) {
                    if ( uxQueueMessagesWaiting( _message_done_queue ) >= 1 ) {
                        xQueueReceive( _message_done_queue, &useless_byte, 0 );
                        _state = MESSAGE;
                        break;
                    }
                }
                break;

            case MESSAGE:
                interpretHeader( type, instruction, sensor_id, data_type, bytes_amount );
                if ( type == packet_t::INST ) {
                    listener->receivedINST( instruction, data_array );
                } else if ( type == packet_t::UPDATE ) {
                    listener->receivedUPDATE( data_type, data_array );
                } else if ( type == packet_t::SENS ) {
                    float sens_data = data_array[0] + ( data_array[1] * 0.01f );
                    listener->receivedSENS( sensor_id, sens_data );
                }
                clear();
                _state = READ;
                break;
        }
    }
}

MessageInterpreter::~MessageInterpreter() {
    vQueueDelete( _packets_queue );
    vQueueDelete( _message_done_queue );
}

void MessageInterpreter::staticRun( void *pvParameters ) {
    MessageInterpreter *message_interpreter = reinterpret_cast<MessageInterpreter *>( pvParameters );
    message_interpreter->run();
    vTaskDelete( message_interpreter->_this_task_handle );
}

}  // namespace sen