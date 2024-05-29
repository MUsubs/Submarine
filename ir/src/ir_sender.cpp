#include "ir_sender.hpp"

namespace sen {

IrSender::IrSender( int pin ) :
    _pin( pin )
{
    pinMode( _pin, OUTPUT | ANALOG );
    analogWriteFrequency( 38000 );
}

void IrSender::sendSignal( Signal signal ) {
    // set pin on or off, depending on signal object
    analogWrite( _pin, signal.on ? 127 : 0 ); 
    // wait signal.us microseconds
    int64_t wait_start = esp_timer_get_time();
    while ( esp_timer_get_time() < wait_start+signal.us )
        vPortYield();
    // turn pin off
    analogWrite( _pin, 0 );
}

} // namespace sen