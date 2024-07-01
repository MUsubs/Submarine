#ifndef R2D2_PACKET_ENUMS_HPP
#define R2D2_PACKET_ENUMS_HPP

#include <Arduino.h>

#include <array>
#include <map>

namespace sen {

/**
 * @enum sen::packet_t
 * Types of data packets
 */
enum class packet_t { INST = 0b01, UPDATE = 0b10, SENS = 0b11, NONE = 0b0 };

/**
 * @enum sen::inst_t
 * Types of instruction packets
 */
enum class inst_t { NEW_POS = 0b001, ARRIVED = 0b010, STOP = 0b100, ACK = 0b111, NONE = 0b0 };

/**
 * @enum sen::data_t
 * Types of data fields in update packets
 */
enum class data_t { CURR = 0b001, NONE = 0b0 };

/**
 * @enum sen::sens_t
 * Types of sensor packets
 */
enum class sens_t { TEMP = 0b01, GYRO = 0b10, NONE = 0b0 };

/**
 * @brief
 * Map with String keys and their associated header byte
 * i.e. "ARRIVED" is an instruction packet 01 with code 010 and 0 bytes : 0b01010000
 */
extern std::map<String, uint8_t> _single_byte_commands;

/**
 * @struct InstPacket_t packet_enums.hpp "include/packet_enums.hpp"
 * Instruction packet struct with inst_t and following data bytes
 */
struct InstPacket_t {
    inst_t inst_type;
    std::array<uint8_t, 3> data_bytes;

    InstPacket_t( inst_t inst_type, std::array<uint8_t, 3>& data ) :
        inst_type{ inst_type }, data_bytes{ data } {
    }

    InstPacket_t() : inst_type{}, data_bytes{} {
    }
};

/**
 * @struct UpdatePacket_t packet_enums.hpp "include/packet_enums.hpp"
 * Update packet struct with data_t and following data bytes
 */
struct UpdatePacket_t {
    data_t data_type;
    std::array<uint8_t, 3> data_bytes;

    UpdatePacket_t( data_t data_type, std::array<uint8_t, 3>& data ) :
        data_type{ data_type }, data_bytes{ data } {
    }

    UpdatePacket_t() : data_type{}, data_bytes{} {
    }
};

/**
 * @struct SensPacket_t packet_enums.hpp "include/packet_enums.hpp"
 * Sensor packet struct with sens_t and following float data
 */
struct SensPacket_t {
    sens_t sens_id;
    float data;

    SensPacket_t( sens_t sensor_id, float data ) : sens_id( sensor_id ), data( data ) {
    }

    SensPacket_t() : sens_id{}, data{} {
    }
};

}  // namespace sen

#endif  // R2D2_PACKET_ENUMS_HPP