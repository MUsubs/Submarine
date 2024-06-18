#ifndef R2D2_DUMMY_DATA_SENDER
#define R2D2_DUMMY_DATA_SENDER

#include <Arduino.h>

#include <deque>

#include "dummy_sen_types.hpp"

namespace sen {

class DummyDataSender {
public:
    DummyDataSender( int led_pin, int freq );
    void sendBytes( const std::deque<uint8_t>& bytes );

    uint8_t generateInstructionHeader( Inst_t inst, uint8_t n_bytes );
    uint8_t generateUpdateHeader( Data_t data_id, uint8_t n_bytes );
    uint8_t generateSensorHeader( Sens_t sensor, uint8_t n_bytes );

private:
    int _led_pin;
    unsigned int _freq;
};

}  // namespace sen
#endif  // R2D2_DUMMY_DATA_SENDER