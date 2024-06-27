#include "packet_enums.hpp"

namespace sen {

std::map<String, uint8_t> _single_byte_commands{
    { "ARRIVED",
      ( static_cast<uint8_t>( packet_t::INST ) << 6 ) | ( static_cast<uint8_t>( inst_t::ARRIVED ) << 3 ) },
    { "STOP",
      ( static_cast<uint8_t>( packet_t::INST ) << 6 ) | ( static_cast<uint8_t>( inst_t::STOP ) << 3 ) },
    { "ACK",
      ( static_cast<uint8_t>( packet_t::INST ) << 6 ) | ( static_cast<uint8_t>( inst_t::ACK ) << 3 ) } };

}  // namespace sen