#include "ir_sender.hpp"

#include <Arduino.h>
#include <FreeRTOS.h>

namespace sen {

IrSender::IrSender( int pin ) :
    _pin( pin )
{

    pinMode( _pin, OUTPUT );
    analogWriteFreq( 38000 );
    analogWriteRange( 256 );
}

void IrSender::sendSignal( Signal signal ) {
    // set pin on or off, depending on signal object
    // Serial.printf( "sen t: %i, o: %i\n", signal.us, signal.on );
    analogWrite( _pin, signal.on ? 127 : 0 ); 
    // wait signal.us microseconds
    int64_t wait_start = time_us_64();
    while ( time_us_64() < wait_start+signal.us )
        yield();
    // turn pin off
    analogWrite( _pin, 0 );
}

} // namespace sen