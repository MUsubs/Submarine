#ifndef R2D2_IR_CONTROL_HPP
#define R2D2_IR_CONTROL_HPP

#include "ir_sender.hpp"

namespace sen {

class SendIrControl {
public:
    SendIrControl( int pin, int msg_queue_length );
    void main();
    void sendMessage( uint32_t msg );
    ~SendIrControl();
private:
    QueueHandle_t _msg_queue;
    IrSender _ir_sender;
};

} // namespace sen

#endif // R2D2_IR_CONTROL_HPP