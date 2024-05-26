#ifndef R2D2_IR_RECEIVER_HPP
#define R2D2_IR_RECEIVER_HPP

#include "Arduino.h"

namespace sen {

class IrListener {
public:
    virtual void pauseDetected( uint32_t us ) = 0;
    virtual void signalDetected( uint32_t us ) = 0;
};


class IrReceiver {
public:
    IrReceiver( int pin );
    void setIrListener( IrListener * ir_listener );
    void main();
private:
    enum { wait_for_pause, wait_for_signal } _state = wait_for_pause;
    int _pin;
    IrListener * _ir_listener;
};

} // namespace sen


#endif // R2D2_IRListener_HPP