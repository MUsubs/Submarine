#ifndef R2D2_MESSAGE_PASSER_HPP
#define R2D2_MESSAGE_PASSER_HPP

#include <FreeRTOS.h>
#include <semphr.h>

#include "message_interpreter_listener.hpp"
#include "packet_enums.hpp"
#include "serial_control.hpp"

namespace sen {

/**
 * @class MessagePasser message_passer.hpp "include/message_passer.hpp"
 * @brief Pass Messages from MessageInterpreter to SerialControl
 * @details
 * Goals:
 * 1. Receive Messages from MessageInterpreter (Through interface MessageInterpreterListener)
 * 2. Call appropriate methods in SerialControl according to received messages
 */
class MessagePasser : public MessageInterpreterListener {
public:
    /**
     * @brief Construct a new Message Passer object
     * 
     * @param serial_control Reference to SerialControl instance
     * @param task_priority FreeRTOS task priority
     */
    MessagePasser( SerialControl& serial_control, int task_priority );

    /**
     * @brief
     * Activate SubControl task, put instance into READING state
     * resuming FreeRTOS task
     */
    void activate();

    /**
     * @brief
     * Deactivate SubControl task, put instance into IDLE state
     * suspending FreeRTOS task
     */
    void deactivate();

    /**
     * @see MessageInterpreterListener
     */
    void receivedINST( inst_t inst_type, std::array<uint8_t, 3>& data ) override;

    /**
     * @see MessageInterpreterListener
     */
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