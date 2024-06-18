#ifndef R2D2_DUMMY_SEN_TYPES_HPP
#define R2D2_DUMMY_SEN_TYPES_HPP

namespace sen {

enum Packet_t { INST = 0b01, UPDATE = 0b10, SENS = 0b11 };
enum Inst_t { NEW_POS = 0b001, ARRIVED = 0b010, STOP = 0b100, ACK = 0b111 };
enum Data_t { CURR = 0b001 };
enum Sens_t { TEMP = 0b01, GYRO = 0b10 };

}  // namespace sen
#endif  // R2D2_DUMMY_SEN_TYPES_HPP