#ifndef R2D2_IR_CONSTANTS_HPP
#define R2D2_IR_CONSTANTS_HPP

#include "Arduino.h"

namespace sen {
namespace constants {

// im assuming global/constant variables should be UPPER_SNAKE_CASE, but technically not in code style file

// optimal values used for sending
extern const uint16_t LEADSIGNAL_US;
extern const uint16_t LEADPAUSE_US;
extern const uint16_t BITSIGNAL_US;
extern const uint16_t BITPAUSE_ZERO_US;
extern const uint16_t BITPAUSE_ONE_US;
extern const uint16_t END_SIGNAL_US;

// ranges of values for receiving
extern const uint16_t MAX_PAUSE_US;
extern const uint16_t LEADSIGNAL_MIN_US;
extern const uint16_t LEADSIGNAL_MAX_US;
extern const uint16_t LEADPAUSE_MIN_US;
extern const uint16_t LEADPAUSE_MAX_US;
extern const uint16_t BITPAUSE_MIN_US;
extern const uint16_t BITPAUSE_MAX_US;
extern const uint16_t BITPAUSE_THRESHOLD_ZERO_ONE;

} // namespace constants
} // namespace sen


#endif // R2D2_IR_CONSTANTS_HPP