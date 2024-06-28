#ifndef R2D2_MESSAGE_PASSER_HPP
#define R2D2_MESSAGE_PASSER_HPP

#include <FreeRTOS.h>
#include <semphr.h>

#include "message_interpreter_listener.hpp"
#include "packet_enums.hpp"
#include "serial_control.hpp"

namespace sen {

class MessagePasser : public MessageInterpreterListener {
public:
    MessagePasser( SerialControl& serial_control, int task_priority );

    void activate();

    void deactivate();

    void receivedINST( inst_t inst_type, std::array<uint8_t, 3>& data ) override;

    void receivedSENS( sens_t sensor, float data ) override;

private:
    SerialControl& serial_control;

    enum class state_t { IDLE, READ, SEND };
    state_t _state = state_t::IDLE;

    xTaskHandle _this_task_handle;

    xQueueHandle _inst_queue;
    xQueueHandle _sens_queue;

    void run();

    static void staticRun( void* pvParameters );
};

}  // namespace sen
#endif  // R2D2_MESSAGE_PASSER_HPP