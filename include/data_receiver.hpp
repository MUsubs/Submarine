#ifndef R2D2_DATA_RECEIVER_HPP
#define R2D2_DATA_RECEIVER_HPP

#include <cstdint>

namespace sen {


class MessageInterpreter;

class DataReceiver{
public:
    DataReceiver( int frequency );
    void signalDetected( uint32_t us );
    void pauseDetected( uint32_t us );
    void setListener( MessageInterpreter * listener );
private:
    enum {
        wait_for_bit_signal,
        wait_for_bit_pause
    } _state = wait_for_bit_signal;
    int _unit_us;
    uint8_t _byte = 0;
    MessageInterpreter * _listener = nullptr;
};

} // namespace sen

#endif // R2D2_DATA_RECEIVER_HPP