#include "message_decoder.hpp"

namespace sen {

MessageDecoder::MessageDecoder( IrReceiver & ir ) {
    ir.setIrListener( this );
}


void MessageDecoder::signalDetected( uint32_t us ) {
    // if currently waiting for a lead signal
    if ( _state == wait_for_lead_signal ) {
        // and the signal is the correct length for a lead signal
        if ( us > constants::LEADSIGNAL_MIN_US && us < constants::LEADSIGNAL_MAX_US ) {
            // wait for a lead pause
            _state = wait_for_lead_pause;
        }
    }
}


void MessageDecoder::pauseDetected( uint32_t us ) {
    // if currently waiting for a lead pause
    if ( _state == wait_for_lead_pause ) {
        // if the pause is the correct length for a lead pause, proceed to wait for bit pause
        // otherwise, go back to waiting for a lead signal
        if ( us > constants::LEADPAUSE_MIN_US && us < constants::LEADPAUSE_MAX_US ) {
            _message = 0;
            _state = wait_for_bit_pause;
        } else {
            _state = wait_for_lead_signal;
        }
    // if currently waiting for a bit pause
    } else if ( _state == wait_for_bit_pause ) {
        // if the pause is correct length for a bit pause
        if ( us > constants::BITPAUSE_MIN_US && us < constants::BITPAUSE_MAX_US ) {
            // depending on pause length, append a one or zero to the message
            if ( us > constants::BITPAUSE_THRESHOLD_ZERO_ONE ) {
                _message <<= 1;
                _message |= 1;
            } else {
                _message <<= 1;
            }
            _state = wait_for_bit_pause;
        // if there was a pause, but not the correct length for a bit
        } else {
            // the message has ended, send to listener
            // also, go back to waiting for lead signals
            _message_listener->messageReceived( _message );
            _state = wait_for_lead_signal;
        }
    }
}


void MessageDecoder::setMessageListener( MessageListener * message_listener ) {
    _message_listener = message_listener;
}

} // namespace sen