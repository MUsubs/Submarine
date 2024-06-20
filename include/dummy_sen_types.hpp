#ifndef R2D2_DUMMY_SEN_TYPES_HPP
#define R2D2_DUMMY_SEN_TYPES_HPP

#include <Arduino.h>

#include <array>
#include <map>

namespace sen {

enum packet_t { INST = 0b01, UPDATE = 0b10, SENS = 0b11 };
enum inst_t { NEW_POS = 0b001, ARRIVED = 0b010, STOP = 0b100, ACK = 0b111, NONE = NULL };
enum data_t { CURR = 0b001, NONE = NULL };
enum sens_t { TEMP = 0b01, GYRO = 0b10 };

extern std::map<String, uint8_t> _single_byte_commands;

struct InstPacket_t {
    inst_t inst_type;
    std::array<uint8_t, 10>& data_bytes;

    InstPacket_t( inst_t inst_type, std::array<uint8_t, 10>& data ) :
        inst_type{ inst_type }, data_bytes{ data } {
    }
};

struct UpdatePacket_t {
    data_t data_type;
    std::array<uint8_t, 10>& data_bytes;

    UpdatePacket_t( data_t data_type, std::array<uint8_t, 10>& data ) :
        data_type{ data_type }, data_bytes{ data } {
    }
}

}  // namespace sen
#endif  // R2D2_DUMMY_SEN_TYPES_HPP