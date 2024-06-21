#ifndef R2D2_SUB_CONTROL_HPP
#define R2D2_SUB_CONTROL_HPP

#define R2D2_DEBUG_ENABLE

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>

#include <array>

#include "dummy_data_sender.hpp"
#include "dummy_sen_types.hpp"
#include "dummy_thermo_sensor.hpp"
#include "dummy_travel_control.hpp"
#include "r2d2_debug_macros.hpp"

namespace sen {

class SubControl {
public:
    SubControl(
        DummyTravelControl& travel_control, DummyDataSender& data_sender,
        DummyThermoSensor& thermo_sensor, int task_priority );
    void activate();
    void deactivate();

    void receivedINST( inst_t inst_type );
    void receivedINST( inst_t inst_type, std::array<uint8_t, 3>& data );
    void receivedINST( InstPacket_t& inst_p );

    void receivedUPDATE( data_t data_type, std::array<uint8_t, 3>& data );
    void receivedUPDATE( UpdatePacket_t& update_p );

private:
    DummyTravelControl& _travel_control;
    DummyDataSender& _data_sender;
    DummyThermoSensor& _thermo_sensor;

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