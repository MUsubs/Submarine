#ifndef R2D2_DUMMY_DATA
#define R2D2_DUMMY_DATA

#include <Arduino.h>

#include <bitset>
#include <deque>

#include "message_interpreter.hpp"

namespace sen {

class DummyData {
public:
    DummyData( MessageInterpreter *message_interpreter );

private:
    enum state_t { IDLE, SEND };
    state_t _state = IDLE;

    MessageInterpreter * message_interpreter;
    xTaskHandle _this_task_handle;

    void run();
    static void staticRun( void* pvParameters );
};

}  // namespace sen
#endif  // R2D2_DUMMY_DATA