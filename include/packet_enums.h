#ifndef R2D2_PACKET_ENUMS_H
#define R2D2_PACKET_ENUMS_H

#include <Arduino.h>

#include <map>

namespace sen {

enum class packet_t { INST = 0b01, UPDATE = 0b10, SENS = 0b11 };
enum class inst_t { NEW_POS = 0b001, ARRIVED = 0b010, STOP = 0b100, ACK = 0b111 };
enum class data_t { CURR = 0b001 };
enum class sens_t { TEMP = 0b01, GYRO = 0b10 };

extern std::map<String, uint8_t> _single_byte_commands;
}  // namespace sen

#endif  // R2D2_PACKET_ENUMS_H