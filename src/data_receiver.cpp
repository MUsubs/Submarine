#include "data_receiver.hpp"
#include "message_interpreter.hpp"

#include <Arduino.h>

namespace sen {

const float IMPRECISION_FACTOR = .25;
const float MAX_FACTOR = (1+IMPRECISION_FACTOR);
const float MIN_FACTOR = (1-IMPRECISION_FACTOR);

DataReceiver::DataReceiver( int unit_us ) :
    _unit_us( 1'000'000 / unit_us )
{ }


void DataReceiver::signalDetected( uint32_t us ) {
    // if currently waiting for a bit pause
    if ( _state == wait_for_bit_signal ) {
        // if the pause is correct length for a bit pause
        if ( us > _unit_us * MIN_FACTOR / 2 && us < _unit_us * MAX_FACTOR ) {
            // depending on pause length, append a one or zero to the message
            if ( us < ( _unit_us + _unit_us / 2 ) / 2 ) {
                _byte <<= 1;
            } else {
                _byte <<= 1;
                _byte |= 1;
            }
            _state = wait_for_bit_pause;
        }
    }
}


void DataReceiver::pauseDetected( uint32_t us ) {
    // if currently waiting for a pause
    if ( _state == wait_for_bit_pause ) {
        // and the pause is the correct length for a bit
        if ( us > _unit_us / 2 * MIN_FACTOR && us < _unit_us / 2 * MAX_FACTOR ) {
            // wait for a signal
            _state = wait_for_bit_signal;
        }
        // byte ended if pause between 2 and 4 units
        else if ( us > _unit_us * 2 * MIN_FACTOR && us < _unit_us * 4 ) {
            _listener->byteReceived( _byte );
            _byte = 0;
            _state = wait_for_bit_signal;
        }
        // message ended if pause more than 4 units
        else if ( us > _unit_us * 4 ) {
            _listener->messageDone();
            _byte = 0;
            _state = wait_for_bit_signal;
        }
    }
}


void DataReceiver::setListener( MessageInterpreter * _listener ) {
    _listener = _listener;
}

} // namespace sen