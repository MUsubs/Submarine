#ifndef R2D2_DATA_TRANSCEIVER_HPP
#define R2D2_DATA_TRANSCEIVER_HPP

#include <Arduino.h>
#include <FreeRTOS.h>
#include <LoRa.h>
#include <SPI.h>
#include <queue.h>

#include <deque>
#include <vector>

#include "message_interpreter.hpp"
#include "packet_enums.hpp"

namespace sen {

const uint8_t SUB_LORA_ADDRESS = 0x51;
const uint8_t LAND_LORA_ADDRESS = 0x11;

class DataTransceiver {
public:
    DataTransceiver(
        int nss_pin, int rst_pin, int dio0_pin, bool is_sub, MessageInterpreter& message_interpreter,
        int task_priority );

    void activate();
    void deactivate();

    void sendBytes( std::vector<uint8_t>& bytes );
    uint8_t generateInstructionHeader( inst_t inst, uint8_t n_bytes );
    uint8_t generateUpdateHeader( data_t data_id, uint8_t n_bytes );
    uint8_t generateSensorHeader( sens_t sensor, uint8_t n_bytes );

private:
    MessageInterpreter& message_interpreter;

    int nss_pin;
    int rst_pin;
    int dio0_pin;

    uint8_t sub_address = SUB_LORA_ADDRESS;
    uint8_t land_address = LAND_LORA_ADDRESS;
    bool is_sub;

    xQueueHandle _byte_queue;
    xTaskHandle _this_task_handle;

    enum class state_t { IDLE, COMMAND, SEND, READ };
    state_t _state = state_t::IDLE;

    void run();
    static void staticRun( void* pvParameters );
    void passMessages();
    void writeMessage( const std::vector<uint8_t>& bytes );
};
}  // namespace sen

#endif  // R2D2_DATA_TRANSCEIVER_HPP
