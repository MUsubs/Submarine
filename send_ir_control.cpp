#include "send_ir_control.hpp"
#include "ir_constants.hpp"

namespace sen {


SendIrControl::SendIrControl(
    int pin,
    int msg_queue_length
) :
    _ir_sender( pin ),
    _msg_queue( xQueueCreate( msg_queue_length, sizeof(uint32_t) ) )
{ }

void SendIrControl::main() {
    while ( true ) {
        uint32_t msg;
        xQueueReceive( _msg_queue, &msg, portMAX_DELAY );
        
        _ir_sender.sendSignal( { .us=constants::LEADSIGNAL_US, .on=true } );
        _ir_sender.sendSignal( { .us=constants::LEADPAUSE_US, .on=false } );

        for ( int i=0; i<32; i++ ){
            _ir_sender.sendSignal( { .us=constants::BITSIGNAL_US, .on=true } );
            if ( (msg<<i) & 0x80000000 )
                _ir_sender.sendSignal( { .us=constants::BITPAUSE_ONE_US, .on=false } );
            else
                _ir_sender.sendSignal( { .us=constants::BITPAUSE_ZERO_US, .on=false } );
        }
        
        _ir_sender.sendSignal( { .us=constants::END_SIGNAL_US, .on=true } );
    }
}

void SendIrControl::sendMessage( uint32_t msg ) {
    xQueueSend( _msg_queue, &msg, 0 );
}

SendIrControl::~SendIrControl() {
    vQueueDelete( _msg_queue );
}


} // namespace sen