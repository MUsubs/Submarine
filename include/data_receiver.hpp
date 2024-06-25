#ifndef R2D2_DATA_RECEIVER_HPP
#define R2D2_DATA_RECEIVER_HPP

#include <cstdint>

namespace sen {


class MessageInterpreter;

/**
 * @class DataReceiver data_receiver.hpp
 * @brief Class for turning a series of signals and pauses into bits and bytes
 * @details
 * ### Class goals:
 * - Turn a series of signals and pauses into bits
 * 
 * - Append the identified bits into bytes
 *
 * - Send identified bytes to a MessageInterpreter listener
 * 
 * @see MessageInterpreter
 */
class DataReceiver{
public:
    /**
     * @brief Construct a DataReceiver object
     *
     * @param frequency Frequency (bits/sec) that is being used
     */
    DataReceiver( int frequency );
    /**
     * @brief Called when PhotoDiode has detected a signal
     *
     * @param us Length of the signal in microseconds
     * 
     * @see PhotoDiode
     */
    void signalDetected( uint32_t us );
    /**
     * @brief Called when PhotoDiode has detected a pause
     *
     * @param us Length of the pause in microseconds
     * 
     * @see PhotoDiode
     */
    void pauseDetected( uint32_t us );
    /**
     * @brief Set a MessageInterpreter as listener
     * 
     * @param listener Pointer to the MessageInterpreter
     * 
     * @see MessageInterpreter
     */
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