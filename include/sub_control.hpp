#ifndef R2D2_SUB_CONTROL_HPP
#define R2D2_SUB_CONTROL_HPP

#define R2D2_DEBUG_ENABLE

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>

#include <array>

#include "data_transceiver.hpp"
#include "message_interpreter_listener.hpp"
#include "r2d2_debug_macros.hpp"
#include "thermo_sensor.hpp"
#include "travel_control.hpp"

namespace sen {

/**
 *
 * @class SubControl sub_control.hpp "include/sub_control.hpp"
 * @brief Main Control class of Submarine microcontroller
 * @details
 * Goals :
 * 1. Handle gotten instructions from MessageInterpreter
 * 2. Handle gotten updates from MessageInterpreter
 *
 * This class gets instruction and update packets from MessageInterpreter
 * It orchestrates what TravelControl, DataSender, and ThermoSensor do
 * based on the current state of the submarine (Idle, Receive new position,
 * Travel and receive updates, Measure and send measurement)
 */
class SubControl : public MessageInterpreterListener {
public:
    /**
     * @brief Construct a new SubControl object
     *
     * @param travel_control Reference to TravelControl instance
     * @param data_sender Reference to DataSender instance
     * @param thermo_sensor Reference to ThermoSensor instance
     * @param task_priority FreeRTOS priority
     */
    SubControl(
        asn::TravelControl& travel_control, DataTransceiver& data_sender, ThermoSensor& thermo_sensor,
        int task_priority );

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
    void receivedINST( InstPacket_t& inst_p ) override;

    /**
     * @see MessageInterpreterListener
     */
    void receivedINST( inst_t inst_type, std::array<uint8_t, 3>& data ) override;

    /**
     * @see MessageInterpreterListener
     */
    void receivedINST( inst_t inst_type ) override;

    /**
     * @see MessageInterpreterListener
     */
    void receivedUPDATE( UpdatePacket_t& update_p ) override;

    /**
     * @see MessageInterpreterListener
     */
    void receivedUPDATE( data_t data_type, std::array<uint8_t, 3>& data ) override;

private:
    asn::TravelControl& _travel_control;
    DataTransceiver& _data_sender;
    ThermoSensor& _thermo_sensor;

    xTaskHandle _this_task_handle;
    xQueueHandle _inst_queue;
    xQueueHandle _update_queue;

    enum class state_t { IDLE, INST, TRAVEL, SENS, WAIT_ACK };
    state_t _state = state_t::IDLE;

    void run();
    static void staticRun( void* pvParameters );
};

}  // namespace sen
#endif  // R2D2_SUB_CONTROL_HPP