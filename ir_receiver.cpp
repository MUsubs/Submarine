#include "ir_receiver.hpp"
#include "ir_constants.hpp"

namespace sen {

IrReceiver::IrReceiver( int pin ):
    _pin( pin )
{
    pinMode( _pin, INPUT );
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
            // wait 100 us
            wait_start = esp_timer_get_time();
            while ( esp_timer_get_time() < wait_start+100 )
                vPortYield();
            // if signal still is
            if ( !digitalRead( _pin ) ) {
                // keep track of signal length
                us += 100;
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
            // wait 100 us
            wait_start = esp_timer_get_time();
            while ( esp_timer_get_time() < wait_start+100 )
                vPortYield();
            // if still no signal
            if ( digitalRead( _pin ) ) {
                // keep track of pause length
                us += 100;
                // if the pause exceeds max pause length, notify listener anyway
                if ( us > constants::MAX_PAUSE_US ) {
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