#ifndef R2D2_MESSAGE_DECODER_HPP
#define R2D2_MESSAGE_DECODER_HPP

#include "ir_receiver.hpp"

namespace sen {

class MessageListener {
public:
    virtual void messageReceived( uint8_t msg ) = 0;
    virtual void messageDone() = 0;
};

class MessageDecoder : public IrListener {
public:
    MessageDecoder( IrReceiver & ir, int definition_us );
    void signalDetected( uint32_t us ) override;
    void pauseDetected( uint32_t us ) override;
    void setMessageListener( MessageListener * message_listener );
private:
    enum {
        wait_for_bit_signal,
        wait_for_bit_pause
    } _state = wait_for_bit_signal;
    int _definition_us;
    MessageListener * _message_listener;
    uint32_t _message;
};

}


#endif