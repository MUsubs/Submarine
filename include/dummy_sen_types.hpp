#ifndef R2D2_DUMMY_SEN_TYPES_HPP
#define R2D2_DUMMY_SEN_TYPES_HPP

namespace sen {

enum packet_t { INST = 0b01, UPDATE = 0b10, SENS = 0b11 };
enum inst_t { NEW_POS = 0b001, ARRIVED = 0b010, STOP = 0b100, ACK = 0b111 };
enum data_t { CURR = 0b001 };
enum sens_t { TEMP = 0b01, GYRO = 0b10 };

}  // namespace sen
#endif  // R2D2_DUMMY_SEN_TYPES_HPP