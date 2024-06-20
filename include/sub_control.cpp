#include "sub_control.hpp"

namespace sen {

SubControl::SubControl(
    DummyTravelControl& travel_control, DummyDataSender& data_sender,
    DummyThermoSensor& thermo_sensor ) :
    _travel_control{ travel_control }, _data_sender{ data_sender },
    _thermo_sensor( thermo_sensor ), _this_task_handle{},
    _inst_queue{ xQueueCreate( 10, sizeof( InstPacket_t ) ) },
    _update_queue{ xQueueCreate( 20, sizeof( UpdatePacket_t ) ) } {
}

void SubControl::activate() {
    vTaskResume( _this_task_handle );
    _state = state_t::INST;
}

void SubControl::deactivate() {
    vTaskSuspend( _this_task_handle );
    _state = state_t::IDLE;
}

void SubControl::receivedINST( inst_t inst_type, std::array<uint8_t, 10>& data ) {
    InstPacket_t inst_packet{ inst_type, data };
    InstPacket_t* send_inst_packet = &inst_packet;
    xQueueSend( _inst_queue, (void*)&send_inst_packet, 0 );
}

void SubControl::receivedINST( InstPacket_t& inst_p ) {
    InstPacket_t* send_inst_packet = &inst_p;
    xQueueSend( _inst_queue, (void*)&send_inst_packet, 0 );
}

void SubControl::receivedUPDATE( data_t data_type, std::array<uint8_t, 10>& data ) {
    UpdatePacket_t update_packet{ data_type, data };
    UpdatePacket_t* send_update_packet = &update_packet;
    xQueueSend( _update_queue, (void*)&send_update_packet, 0 );
}

void SubControl::receivedUPDATE( UpdatePacket_t& update_p ) {
    UpdatePacket_t* send_update_packet = &update_p;
    xQueueSend( _update_queue, (void*)&send_update_packet, 0 );
}

void SubControl::run() {
    bool b_reset = false;

    inst_t received_inst_t = inst_t::NONE;
    std::array<uint8_t, 10> received_inst_data;
    InstPacket_t received_inst = { received_inst_t, received_inst_data };

    data_t received_update_t = data_t::NONE;
    std::array<uint8_t, 10> received_update_data;
    UpdatePacket_t received_update = { received_update_t, received_update_data };

    float thermo_measure;

    for ( ;; ) {
        Serial.println( "==TODO== Implement SubControl run" );
        switch ( _state ) {
            case state_t::IDLE:
                b_reset = true;
                vTaskSuspend( _this_task_handle );
                break;

            case state_t::INST:
                if ( b_reset ) {
                    xQueueReset( _inst_queue );
                    xQueueReset( _update_queue );
                    b_reset = false;
                }
                if ( xQueueReceive( _inst_queue, &received_inst, 0 ) ) {
                    if ( received_inst.inst_type == STOP ) {
                        std::deque<uint8_t> send_bytes = {
                            _data_sender.generateInstructionHeader( ACK, 0 ) };
                        _data_sender.sendBytes( send_bytes );
                        b_reset = true;
                        break;
                    } else if ( received_inst.inst_type == NEW_POS ) {
                        float x = received_inst.data_bytes[0] / 255.0;
                        float y = received_inst.data_bytes[1] / 255.0;
                        float z = received_inst.data_bytes[2] / 255.0;
                        _travel_control.newDest( x, y, z );

                        std::deque<uint8_t> send_bytes = {
                            _data_sender.generateInstructionHeader( ACK, 0 ) };
                        _data_sender.sendBytes( send_bytes );

                        xQueueReset( _update_queue );
                        _state = state_t::TRAVEL;
                        break;
                    }
                }
                break;

            case state_t::TRAVEL:
                if ( xQueueReceive( _update_queue, &received_update, 0 ) ) {
                    float x = received_update.data_bytes[0] / 255.0;
                    float y = received_update.data_bytes[1] / 255.0;
                    float z = received_update.data_bytes[2] / 255.0;
                    _travel_control.updateCurPos( x, y, z );
                }
                if ( xQueueReceive( _inst_queue, &received_inst, 0 ) ) {
                    if ( received_inst.inst_type == NEW_POS ) {
                        std::deque<uint8_t> send_bytes = {
                            _data_sender.generateInstructionHeader( ACK, 0 ) };
                        _data_sender.sendBytes( send_bytes );
                        break;
                    } else if ( received_inst.inst_type == STOP ) {
                        _travel_control.stop();
                        std::deque<uint8_t> send_bytes = {
                            _data_sender.generateInstructionHeader( ACK, 0 ) };
                        _data_sender.sendBytes( send_bytes );
                        b_reset = true;
                        _state = state_t::INST;
                        break;
                    } else if ( received_inst.inst_type == ARRIVED ) {
                        _travel_control.stop();
                        std::deque<uint8_t> send_bytes = {
                            _data_sender.generateInstructionHeader( ACK, 0 ) };
                        _data_sender.sendBytes( send_bytes );
                        _state = state_t::SENS;
                        break;
                    }
                    break;
                }
                break;

            case state_t::SENS:
                thermo_measure = _thermo_sensor.getTemperature();
                uint8_t thermo_int = (uint8_t)thermo_measure;
                uint8_t thermo_dec = int( ( thermo_measure - (float)thermo_int ) * 100 );
                std::deque<uint8_t> send_bytes = {
                    _data_sender.generateSensorHeader( sens_t::TEMP, 2 ), thermo_int,
                    thermo_dec };
                _data_sender.sendBytes( send_bytes );
                _state = state_t::WAIT_ACK;
                break;

            case state_t::WAIT_ACK:
                if ( xQueueReceive( _inst_queue, &received_inst, 0 ) ) {
                    if ( received_inst.inst_type == ACK ) {
                        b_reset = true;
                        _state = state_t::INST;
                        break;
                    } else {
                        if ( received_inst.inst_type == STOP ) {
                            std::deque<uint8_t> send_bytes = {
                                _data_sender.generateInstructionHeader( ACK, 0 ) };
                            _data_sender.sendBytes( send_bytes );
                            b_reset = true;
                            _state = state_t::INST;
                            break;
                        } else if ( received_inst.inst_type == ARRIVED ) {
                            std::deque<uint8_t> send_bytes = {
                                _data_sender.generateInstructionHeader( ACK, 0 ) };
                            _data_sender.sendBytes( send_bytes );
                            break;
                        } else {
                            _state = state_t::SENS;
                            break;
                        }
                    }
                }
                break;

            default:
                Serial.println( "==ERROR== Invalid state in SubControl" );
                break;
        }
    }
}

void SubControl::staticRun( void* pvParameters ) {
    SubControl* sub_control = reinterpret_cast<SubControl*>( pvParameters );
    sub_control->run();
    vTaskDelete( sub_control->_this_task_handle );
}

}  // namespace sen