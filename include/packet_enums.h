#ifndef R2D2_PACKET_ENUMS_H
#define R2D2_PACKET_ENUMS_H

namespace sen {

enum class packet_t { INST = 0b01, UPDATE = 0b10, SENS = 0b11, NONE = 0b0 };
enum class inst_t { NEW_POS = 0b001, ARRIVED = 0b010, STOP = 0b100, ACK = 0b111, NONE = 0b0 };
enum class data_t { CURR = 0b001, NONE = 0b0 };
enum class sens_t { TEMP = 0b01, GYRO = 0b10, NONE = 0b0 };

}  // namespace sen

#endif  // R2D2_PACKET_ENUMS_H