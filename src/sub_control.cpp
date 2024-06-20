#include "sub_control.hpp"

namespace sen {

SubControl::SubControl(
    DummyTravelControl& travel_control, DummyDataSender& data_sender,
    DummyThermoSensor& thermo_sensor, int task_priority ) :
    _travel_control{ travel_control }, _data_sender{ data_sender }, _thermo_sensor( thermo_sensor ),
    _this_task_handle{}, _inst_queue{ xQueueCreate( 10, sizeof( InstPacket_t ) ) },
    _update_queue{ xQueueCreate( 20, sizeof( UpdatePacket_t ) ) } {
    if ( !Serial ) Serial.begin( 9600 );
    xTaskCreate( staticRun, "SUB_CONTROL", 4000, (void*)this, task_priority, &_this_task_handle );
}

void SubControl::activate() {
    R2D2_DEBUG_LOG( "Activating SubControl instance" );
    vTaskResume( _this_task_handle );
    _state = state_t::INST;
}

void SubControl::deactivate() {
    R2D2_DEBUG_LOG( "Deactivating SubControl instance" );
    vTaskSuspend( _this_task_handle );
    _state = state_t::IDLE;
}

void SubControl::receivedINST( inst_t inst_type, std::array<uint8_t, 10>& data ) {
    R2D2_DEBUG_LOG(
        "Received Instruction of type %d, with %d arguments", (int)inst_type, data.size() );
    InstPacket_t inst_packet{ inst_type, data };
    InstPacket_t* send_inst_packet = &inst_packet;
    xQueueSend( _inst_queue, (void*)&send_inst_packet, 0 );
}

void SubControl::receivedINST( InstPacket_t& inst_p ) {
    R2D2_DEBUG_LOG(
        "Received Instruction of type %d, with %d arguments", (int)inst_p.inst_type,
        inst_p.data_bytes.size() );
    InstPacket_t* send_inst_packet = &inst_p;
    xQueueSend( _inst_queue, (void*)&send_inst_packet, 0 );
}

void SubControl::receivedUPDATE( data_t data_type, std::array<uint8_t, 10>& data ) {
    R2D2_DEBUG_LOG( "Received Update of type %d, with %d arguments", (int)data_type, data.size() );
    UpdatePacket_t update_packet{ data_type, data };
    UpdatePacket_t* send_update_packet = &update_packet;
    xQueueSend( _update_queue, (void*)&send_update_packet, 0 );
}

void SubControl::receivedUPDATE( UpdatePacket_t& update_p ) {
    R2D2_DEBUG_LOG(
        "Received Update of type %d, with %d arguments", (int)update_p.data_type,
        update_p.data_bytes.size() );
    UpdatePacket_t* send_update_packet = &update_p;
    xQueueSend( _update_queue, (void*)&send_update_packet, 0 );
}

void SubControl::run() {
    bool b_reset = false;

    inst_t received_inst_t = inst_t::INST_NONE;
    std::array<uint8_t, 10> received_inst_data;
    InstPacket_t received_inst = { received_inst_t, received_inst_data };

    data_t received_update_t = data_t::DATA_NONE;
    std::array<uint8_t, 10> received_update_data;
    UpdatePacket_t received_update = { received_update_t, received_update_data };

    float thermo_measure;

    std::deque<uint8_t> send_bytes;

    for ( ;; ) {
        switch ( _state ) {
            // braces after cases necessary because of initialization within cases for
            // sending packets (std::deque)
            case state_t::IDLE: {
                b_reset = true;
                vTaskSuspend( _this_task_handle );
                break;
            }

            case state_t::INST: {
                if ( b_reset ) {
                    R2D2_DEBUG_LOG(
                        "Resetting instruction (%d items) and update (%d items) queue in "
                        "SubControl",
                        uxQueueMessagesWaiting( _inst_queue ),
                        uxQueueMessagesWaiting( _update_queue ) );
                    xQueueReset( _inst_queue );
                    xQueueReset( _update_queue );
                    b_reset = false;
                }
                if ( xQueueReceive( _inst_queue, &received_inst, 0 ) ) {
                    if ( received_inst.inst_type == STOP ) {
                        R2D2_DEBUG_LOG(
                            "State:%d - SubControl Received a STOP instruction", _state );
                        send_bytes = { _data_sender.generateInstructionHeader( ACK, 0 ) };
                        _data_sender.sendBytes( send_bytes );
                        b_reset = true;
                        break;
                    } else if ( received_inst.inst_type == NEW_POS ) {
                        R2D2_DEBUG_LOG(
                            "State:%d - SubControl Received a NEW_POS instruction with args: %d, "
                            "%d, %d",
                            _state, received_inst.data_bytes[0], received_inst.data_bytes[1],
                            received_inst.data_bytes[2] );
                        float x = received_inst.data_bytes[0] / 255.0;
                        float y = received_inst.data_bytes[1] / 255.0;
                        float z = received_inst.data_bytes[2] / 255.0;
                        _travel_control.newDest( x, y, z );

                        send_bytes = { _data_sender.generateInstructionHeader( ACK, 0 ) };
                        _data_sender.sendBytes( send_bytes );

                        xQueueReset( _update_queue );
                        _state = state_t::TRAVEL;
                        break;
                    }
                }
                break;
            }

            case state_t::TRAVEL: {
                if ( xQueueReceive( _update_queue, &received_update, 0 ) ) {
                    R2D2_DEBUG_LOG(
                        "State:%d - SubControl Received an UPDATE with args: %d, %d, %d", _state,
                        received_update.data_bytes[0], received_update.data_bytes[1],
                        received_update.data_bytes[2] );
                    float x = received_update.data_bytes[0] / 255.0;
                    float y = received_update.data_bytes[1] / 255.0;
                    float z = received_update.data_bytes[2] / 255.0;
                    _travel_control.updateCurPos( x, y, z );
                }
                if ( xQueueReceive( _inst_queue, &received_inst, 0 ) ) {
                    if ( received_inst.inst_type == NEW_POS ) {
                        R2D2_DEBUG_LOG(
                            "State:%d - SubControl Received a NEW_POS instruction with args: %d, "
                            "%d, %d",
                            _state, received_inst.data_bytes[0], received_inst.data_bytes[1],
                            received_inst.data_bytes[2] );
                        send_bytes = { _data_sender.generateInstructionHeader( ACK, 0 ) };
                        _data_sender.sendBytes( send_bytes );
                        break;
                    } else if ( received_inst.inst_type == STOP ) {
                        R2D2_DEBUG_LOG(
                            "State:%d - SubControl Received a STOP instruction", _state );
                        _travel_control.stop();

                        send_bytes = { _data_sender.generateInstructionHeader( ACK, 0 ) };
                        _data_sender.sendBytes( send_bytes );
                        b_reset = true;
                        _state = state_t::INST;
                        break;
                    } else if ( received_inst.inst_type == ARRIVED ) {
                        R2D2_DEBUG_LOG(
                            "State:%d - SubControl Received an ARRIVED instruction", _state );
                        _travel_control.stop();

                        send_bytes = { _data_sender.generateInstructionHeader( ACK, 0 ) };
                        _data_sender.sendBytes( send_bytes );
                        _state = state_t::SENS;
                        break;
                    }
                    break;
                }
                break;
            }

            case state_t::SENS: {
                thermo_measure = _thermo_sensor.getTemperature();
                R2D2_DEBUG_LOG(
                    "State:%d - SubControl Measuring and sending. Measurement: %f", _state,
                    thermo_measure );
                uint8_t thermo_int = (uint8_t)thermo_measure;
                uint8_t thermo_dec = int( ( thermo_measure - (float)thermo_int ) * 100 );
                send_bytes = {
                    _data_sender.generateSensorHeader( sens_t::TEMP, 2 ), thermo_int, thermo_dec };
                _data_sender.sendBytes( send_bytes );
                _state = state_t::WAIT_ACK;
                break;
            }

            case state_t::WAIT_ACK: {
                if ( xQueueReceive( _inst_queue, &received_inst, 0 ) ) {
                    if ( received_inst.inst_type == ACK ) {
                        R2D2_DEBUG_LOG(
                            "State:%d - SubControl Received an ACK instruction", _state );
                        b_reset = true;
                        _state = state_t::INST;
                        break;
                    } else {
                        if ( received_inst.inst_type == STOP ) {
                            R2D2_DEBUG_LOG(
                                "State:%d - SubControl Received a STOP instruction", _state );
                            send_bytes = { _data_sender.generateInstructionHeader( ACK, 0 ) };
                            _data_sender.sendBytes( send_bytes );
                            b_reset = true;
                            _state = state_t::INST;
                            break;
                        } else if ( received_inst.inst_type == ARRIVED ) {
                            R2D2_DEBUG_LOG(
                                "State:%d - SubControl Received an ARRIVED instruction", _state );
                            send_bytes = { _data_sender.generateInstructionHeader( ACK, 0 ) };
                            _data_sender.sendBytes( send_bytes );
                            break;
                        } else {
                            _state = state_t::SENS;
                            break;
                        }
                    }
                }
                break;
            }

            default:
                Serial.println( "==ERROR== Invalid state in SubControl" );
                R2D2_DEBUG_LOG( "State = %d", _state );
                break;
        }
    }
}

void SubControl::staticRun( void* pvParameters ) {
    R2D2_DEBUG_LOG( "Starting SubControl run through staticRun" );
    SubControl* sub_control = reinterpret_cast<SubControl*>( pvParameters );
    sub_control->run();
    R2D2_DEBUG_LOG( "SubControl run exited, deleting task" );
    vTaskDelete( sub_control->_this_task_handle );
}

}  // namespace sen