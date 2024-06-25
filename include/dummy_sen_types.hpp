#ifndef R2D2_DUMMY_SEN_TYPES_HPP
#define R2D2_DUMMY_SEN_TYPES_HPP

// #define R2D2_DEBUG_ENABLE

#include <Arduino.h>

#include <array>
#include <map>

#include "r2d2_debug_macros.hpp"

namespace sen {

enum packet_t { INST = 0b01, UPDATE = 0b10, SENS = 0b11 };
enum inst_t { NEW_POS = 0b001, ARRIVED = 0b010, STOP = 0b100, ACK = 0b111, INST_NONE = 0 };
enum data_t { CURR = 0b001, DATA_NONE = 0 };
enum sens_t { TEMP = 0b01, GYRO = 0b10 };

extern std::map<String, uint8_t> _single_byte_commands;

struct InstPacket_t {
    inst_t inst_type;
    std::array<uint8_t, 3> data_bytes;

    InstPacket_t( inst_t inst_type, std::array<uint8_t, 3>& data ) :
        inst_type{ inst_type }, data_bytes{ data } {
        R2D2_DEBUG_LOG( "InstPacket_t created with inst_t = %d", inst_type );
    }

    InstPacket_t() : inst_type{}, data_bytes{} {
    }
};

struct UpdatePacket_t {
    data_t data_type;
    std::array<uint8_t, 3> data_bytes;

    UpdatePacket_t( data_t data_type, std::array<uint8_t, 3>& data ) :
        data_type{ data_type }, data_bytes{ data } {
        R2D2_DEBUG_LOG( "UpdatePacket_t created with data_t = %d", data_type );
    }

    UpdatePacket_t() : data_type{}, data_bytes{} {
    }
};

}  // namespace sen
#endif  // R2D2_DUMMY_SEN_TYPES_HPP