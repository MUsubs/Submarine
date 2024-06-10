#include "send_ir_control.hpp"

namespace sen {


SendIrControl::SendIrControl(
    int pin,
    int unit_length_us,
    int msg_queue_length
) :
    _unit_length_us( unit_length_us ),
    _ir_sender( pin ),
    _msg_queue( xQueueCreate( msg_queue_length, sizeof(uint32_t) ) )
{ }

void SendIrControl::main() {
    while ( true ) {
        uint32_t msg;
        xQueueReceive( _msg_queue, &msg, portMAX_DELAY );
        
        for ( int i=0; i<32; i++ ){
            if ( (msg<<i) & 0x80000000 )
                _ir_sender.sendSignal( { .us=_unit_length_us, .on=true } );
            else
                _ir_sender.sendSignal( { .us=_unit_length_us / 2, .on=true } );
            _ir_sender.sendSignal( { .us=_unit_length_us, .on=false } );
        }
    }
}

void SendIrControl::sendMessage( uint32_t msg ) {
    xQueueSend( _msg_queue, &msg, 0 );
}

SendIrControl::~SendIrControl() {
    vQueueDelete( _msg_queue );
}


} // namespace sen