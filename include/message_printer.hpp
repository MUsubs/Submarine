#ifndef R2D2_MESSAGE_PRINTER_HPP
#define R2D2_MESSAGE_PRINTER_HPP

#include "message_interpreter_listener.hpp"

namespace sen {

class MessagePrinter : public MessageInterpreterListener {
public:
    MessagePrinter() {
        MessageInterpreterListener();
    };
};

}  // namespace sen
#endif  // R2D2_MESSAGE_PRINTER_HPP