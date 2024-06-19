#include "message_decoder.hpp"

#include <Arduino.h>

namespace sen {

const float IMPRECISION_FACTOR = .25;
const float MAX_FACTOR = (1+IMPRECISION_FACTOR);
const float MIN_FACTOR = (1-IMPRECISION_FACTOR);

MessageDecoder::MessageDecoder( IrReceiver & ir, int definition_us ) :
    _definition_us( definition_us )
{
    ir.setIrListener( this );
}


void MessageDecoder::signalDetected( uint32_t us ) {
    // if currently waiting for a bit pause
    if ( _state == wait_for_bit_signal ) {
        // if the pause is correct length for a bit pause
        if ( us > _definition_us * MIN_FACTOR / 2 && us < _definition_us * MAX_FACTOR ) {
            // depending on pause length, append a one or zero to the message
            if ( us < (_definition_us +_definition_us/2) / 2 ) {
                _message <<= 1;
            } else {
                _message <<= 1;
                _message |= 1;
            }
            _state = wait_for_bit_pause;
        }
    }
}


void MessageDecoder::pauseDetected( uint32_t us ) {
    // if currently waiting for a pause
    if ( _state == wait_for_bit_pause ) {
        // and the pause is the correct length
        if ( us > _definition_us / 2 * MIN_FACTOR && us < _definition_us / 2 * MAX_FACTOR ) {
            // wait for a signal
            _state = wait_for_bit_signal;
        }
        // if there was a pause, but at a length more than 4 units
        else if ( us > _definition_us * 4 * MAX_FACTOR ) {
            // the message has ended, send to listener
            // also, go back to waiting for lead signals
            _message_listener->messageReceived( _message );
            _state = wait_for_bit_signal;
        }
    }
}


void MessageDecoder::setMessageListener( MessageListener * message_listener ) {
    _message_listener = message_listener;
}

} // namespace sen