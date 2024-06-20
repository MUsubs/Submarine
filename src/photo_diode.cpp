#include "photo_diode.hpp"
#include "data_receiver.hpp"

#include <Arduino.h>
#include <FreeRTOS.h>

namespace sen {

PhotoDiode::PhotoDiode( int pin, int poll_frequency, int max_pause_us ):
    _pin( pin ),
    _wait_us( 1'000'000 / poll_frequency ),
    _max_pause_us( max_pause_us )
{
    pinMode( _pin, INPUT_PULLDOWN );
}

void PhotoDiode::setListener( DataReceiver * listener ) {
    _listener = listener;
}

void PhotoDiode::main( ) {
    uint32_t us = 0;
    int64_t wait_start;
    while ( true ) {
        switch ( _state ) {
        // if waiting for a pause (so currently receiving signal)
        case wait_for_pause:
            // wait _definition_us us
            wait_start = time_us_64();
            while ( time_us_64() < wait_start+_wait_us )
                yield();
            // if signal still is
            if ( !digitalRead( _pin ) ) {
                // keep track of signal length
                us += _wait_us;
            // if signal not is
            } else {
                // send signal to listener and start waiting for signal
                _listener->signalDetected( us );
                us = 0;
                _state = wait_for_signal;
            }
            break;
        // if waiting for a signal (so currently no signal / a pause)
        case wait_for_signal:
            // wait _definition_us us
            wait_start = time_us_64();
            while ( time_us_64() < wait_start+_wait_us )
                yield();
            // if still no signal
            if ( digitalRead( _pin ) ) {
                // keep track of pause length
                us += _wait_us;
                // if the pause exceeds max pause length, notify listener anyway
                if ( us > _max_pause_us ) {
                    _listener->pauseDetected( us );
                    us = 0;
                }
            // if pause stopped (thus signal started)
            } else {
                // notify listener of pause and start waiting for pause
                _listener->pauseDetected( us );
                us = 0;
                _state = wait_for_pause;
            }
            break;
        }
    }
}

} // namespace sen