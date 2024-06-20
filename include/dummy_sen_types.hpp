#ifndef R2D2_DUMMY_SEN_TYPES_HPP
#define R2D2_DUMMY_SEN_TYPES_HPP

#include <Arduino.h>

#include <map>

namespace sen {

enum packet_t { INST = 0b01, UPDATE = 0b10, SENS = 0b11 };
enum inst_t { NEW_POS = 0b001, ARRIVED = 0b010, STOP = 0b100, ACK = 0b111 };
enum data_t { CURR = 0b001 };
enum sens_t { TEMP = 0b01, GYRO = 0b10 };

extern std::map<String, uint8_t> _single_byte_commands;

}  // namespace sen
#endif  // R2D2_DUMMY_SEN_TYPES_HPP