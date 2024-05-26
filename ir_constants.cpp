#include "ir_constants.hpp"

namespace sen {
namespace constants {

// optimal values used for sending
const uint16_t LEADSIGNAL_US = 9000;
const uint16_t LEADPAUSE_US = 4500;
const uint16_t BITSIGNAL_US = 560;
const uint16_t BITPAUSE_ZERO_US = 560;
const uint16_t BITPAUSE_ONE_US = 1680;
const uint16_t END_SIGNAL_US = 560;

// ranges of values for receiving
const uint16_t MAX_PAUSE_US = 6000;
const uint16_t LEADSIGNAL_MIN_US = 7000;
const uint16_t LEADSIGNAL_MAX_US = 11000;
const uint16_t LEADPAUSE_MIN_US = 3000;
const uint16_t LEADPAUSE_MAX_US = 6000;
const uint16_t BITPAUSE_MIN_US = 200;
const uint16_t BITPAUSE_MAX_US = 2000;
const uint16_t BITPAUSE_THRESHOLD_ZERO_ONE = 1100;

} // namespace constants
} // namespace sen