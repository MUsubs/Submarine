#include "dummy_sen_types.hpp"

namespace sen {

std::map<String, uint8_t> _single_byte_commands{
    { "ARRIVED", ( INST << 6 ) | ( ARRIVED << 3 ) },
    { "STOP", ( INST << 6 ) | ( STOP << 3 ) },
    { "ACK", ( INST << 6 ) | ( ACK << 3 ) } };

}
