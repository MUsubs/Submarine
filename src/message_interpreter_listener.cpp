#include "message_interpreter_listener.hpp"

#include "packet_enums.hpp"

#define R2D2_DEBUG_ENABLE
#include <String.h>

#include "r2d2_debug_macros.hpp"

namespace sen {

void MessageInterpreterListener::receivedINST( InstPacket_t& inst_p ) {
    String packet = "";
    if ( inst_p.inst_type == inst_t::NEW_POS ) {
        packet = "NEW_POS";
        R2D2_DEBUG_LOG(
            "MessageInterpreterListener : Received INST %s, %d, %d, %d", packet.c_str(),
            inst_p.data_bytes[0], inst_p.data_bytes[1], inst_p.data_bytes[2] );
    }
    if ( inst_p.inst_type == inst_t::ARRIVED ) packet = "ARRIVED";
    if ( inst_p.inst_type == inst_t::ACK ) packet = "ACK";
    if ( inst_p.inst_type == inst_t::STOP ) packet = "STOP";
    R2D2_DEBUG_LOG( "MessageInterpreterListener : Received INST %s", packet.c_str() );
}

void MessageInterpreterListener::receivedINST( inst_t inst_type, std::array<uint8_t, 3>& data ) {
    String packet = "";
    if ( inst_type == inst_t::NEW_POS ) {
        packet = "NEW_POS";
        R2D2_DEBUG_LOG(
            "MessageInterpreterListener : Received INST %s, %d, %d, %d", packet.c_str(), data[0], data[1],
            data[2] );
        return;
    }
    if ( inst_type == inst_t::ARRIVED ) packet = "ARRIVED";
    if ( inst_type == inst_t::ACK ) packet = "ACK";
    if ( inst_type == inst_t::STOP ) packet = "STOP";
    R2D2_DEBUG_LOG( "MessageInterpreterListener : Received INST %s", packet.c_str() );
}
void MessageInterpreterListener::receivedINST( inst_t inst_type ) {
    String packet = "";
    if ( inst_type == inst_t::ARRIVED ) packet = "ARRIVED";
    if ( inst_type == inst_t::ACK ) packet = "ACK";
    if ( inst_type == inst_t::STOP ) packet = "STOP";
    R2D2_DEBUG_LOG( "MessageInterpreterListener : Received INST %s", packet.c_str() );
}
void MessageInterpreterListener::receivedUPDATE( UpdatePacket_t& update_p ) {
    String packet = "";
    if ( update_p.data_type == data_t::CURR ) packet = "CURR";
    R2D2_DEBUG_LOG(
        "MessageInterpreterListener : Received UPDATE %s, %d, %d, %d", packet.c_str(),
        update_p.data_bytes[0], update_p.data_bytes[1], update_p.data_bytes[2] );
}

void MessageInterpreterListener::receivedUPDATE( data_t data_type, std::array<uint8_t, 3>& data ) {
    String packet = "";
    if ( data_type == data_t::CURR ) packet = "CURR";
    R2D2_DEBUG_LOG(
        "MessageInterpreterListener : Received UPDATE %s, %d, %d, %d", packet.c_str(), data[0], data[1],
        data[2] );
}

void MessageInterpreterListener::receivedSENS( sens_t sensor, float data ) {
    String packet = "";
    if ( sensor == sens_t::TEMP ) packet = "TEMP";
    if ( sensor == sens_t::GYRO ) packet = "GYRO";
    R2D2_DEBUG_LOG( "MessageInterpreterListener : Received SENS %s, %f", packet.c_str(), data );
}

}  // namespace sen