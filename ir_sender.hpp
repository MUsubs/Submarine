#ifndef R2D2_IR_SENDER_HPP
#define R2D2_IR_SENDER_HPP

#include "Arduino.h"

namespace sen {

struct Signal {
    int us;
    bool on;
};

class IrSender {
public:
    IrSender( int pin );
    void sendSignal( Signal signal );
private:
    int _pin;
};

} // namespace sen


#endif // R2D2_IR_SENDER_HPP