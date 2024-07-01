#include "message_passer.hpp"

namespace sen {

// public
MessagePasser::MessagePasser( SerialControl& serial_control, int task_priority ) :
    serial_control( serial_control ), _this_task_handle{},
    _inst_queue{ xQueueCreate( 3, sizeof( InstPacket_t ) ) },
    _sens_queue{ xQueueCreate( 3, sizeof( SensPacket_t ) ) } {
    xTaskCreate(
        staticRun, "MESSAGE_PASSER", 1000, (void*)this, task_priority, &_this_task_handle );
}

void MessagePasser::activate() {
    _state = state_t::READ;
    vTaskResume( _this_task_handle );
}

void MessagePasser::deactivate() {
    _state = state_t::IDLE;
    vTaskSuspend( _this_task_handle );
}

void MessagePasser::receivedINST( inst_t inst_type, std::array<uint8_t, 3>& data ) {
    InstPacket_t inst_packet{ inst_type, data };
    xQueueSend( _inst_queue, (void*)&inst_packet, 0 );
}

void MessagePasser::receivedSENS( sens_t sensor, float data ) {
    SensPacket_t sens_packet{ sensor, data };
    xQueueSend( _sens_queue, (void*)&sens_packet, 0 );
}

// private
void MessagePasser::run() {
    InstPacket_t incoming_inst;
    SensPacket_t incoming_sens;
    bool b_inst = false;
    bool b_sens = false;
    for ( ;; ) {
        switch ( _state ) {
            case state_t::IDLE:
                vTaskSuspend( _this_task_handle );
                break;

            case state_t::READ:
                if ( xQueueReceive( _inst_queue, &incoming_inst, 0 ) ) {
                    b_inst = true;
                }
                if ( xQueueReceive( _sens_queue, &incoming_sens, 0 ) ) {
                    b_sens = true;
                }
                if ( b_inst || b_sens ) _state = state_t::SEND;
                break;

            case state_t::SEND:
                if ( b_inst ) {
                    if ( incoming_inst.inst_type == inst_t::ACK ) {
                        serial_control.receivedACK();
                    }
                    b_inst = false;
                }
                if ( b_sens ) {
                    serial_control.addMeasure( incoming_sens.data );
                    b_sens = false;
                }
                break;
        }
    }
}

void MessagePasser::staticRun( void* pvParamters ) {
    MessagePasser* message_passer = reinterpret_cast<MessagePasser*>( pvParamters );
    message_passer->run();
    vTaskDelete( message_passer->_this_task_handle );
}

}  // namespace sen
