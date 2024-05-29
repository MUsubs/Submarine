#ifndef R2D2_IR_CONTROL_HPP
#define R2D2_IR_CONTROL_HPP

#include "ir_sender.hpp"

namespace sen {

class SendIrControl {
public:
    SendIrControl( int pin, int unit_length_us, int msg_queue_length );
    void main();
    void sendMessage( uint32_t msg );
    ~SendIrControl();
private:
    QueueHandle_t _msg_queue;
    IrSender _ir_sender;
    int _unit_length_us;
};

} // namespace sen

#endif // R2D2_IR_CONTROL_HPP