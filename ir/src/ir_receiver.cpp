#include "ir_receiver.hpp"

#include <Arduino.h>
#include <FreeRTOS.h>

namespace sen {

IrReceiver::IrReceiver( int pin, int definition_us, int max_pause_us ):
    _pin( pin ),
    _definition_us( definition_us ),
    _max_pause_us( max_pause_us )
{
    pinMode( _pin, INPUT_PULLDOWN );
}

void IrReceiver::setIrListener( IrListener * ir_listener ) {
    this->_ir_listener = ir_listener;
}

void IrReceiver::main( ) {
    uint32_t us = 0;
    int64_t wait_start;
    while ( true ) {
        switch ( _state ) {
        // if waiting for a pause (so currently receiving signal)
        case wait_for_pause:
            // wait _definition_us us
            wait_start = time_us_64();
            while ( time_us_64() < wait_start+_definition_us )
                yield();
            // if signal still is
            if ( !digitalRead( _pin ) ) {
                // keep track of signal length
                us += _definition_us;
            // if signal not is
            } else {
                // send signal to listener and start waiting for signal
                _ir_listener->signalDetected( us );
                us = 0;
                _state = wait_for_signal;
            }
            break;
        // if waiting for a signal (so currently no signal / a pause)
        case wait_for_signal:
            // wait _definition_us us
            wait_start = time_us_64();
            while ( time_us_64() < wait_start+_definition_us )
                yield();
            // if still no signal
            if ( digitalRead( _pin ) ) {
                // keep track of pause length
                us += _definition_us;
                // if the pause exceeds max pause length, notify listener anyway
                if ( us > _max_pause_us ) {
                    _ir_listener->pauseDetected( us );
                    us = 0;
                }
            // if pause stopped (thus signal started)
            } else {
                // notify listener of pause and start waiting for pause
                _ir_listener->pauseDetected( us );
                us = 0;
                _state = wait_for_pause;
            }
            break;
        }
    }
}

} // namespace sen