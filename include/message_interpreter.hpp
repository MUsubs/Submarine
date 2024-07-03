#ifndef R2D2_MESSAGE_INTERPRETER_HPP
#define R2D2_MESSAGE_INTERPRETER_HPP

#include <Arduino.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

#include <array>
#include <cstdint>

#include "message_interpreter_listener.hpp"
#include "packet_enums.hpp"

namespace sen {

#define DATA_ARRAY_SIZE 3

class MessageInterpreter {
public:
    MessageInterpreter( int queue_length, int task_priority );
    ~MessageInterpreter();

    void activate();
    void deactivate();

    void byteReceived( uint8_t msg );
    void messageDone();

    void setListener( MessageInterpreterListener *set_listener );

private:
    enum state_t { IDLE, READ, MESSAGE };
    state_t _state = IDLE;

    QueueHandle_t _packets_queue;
    QueueHandle_t _message_done_queue;
    xTaskHandle _this_task_handle;

    std::array<uint8_t, DATA_ARRAY_SIZE> data_array;

    uint8_t bytes_amount = 0;

    sen::packet_t type = packet_t::NONE;
    sen::inst_t instruction = inst_t::NONE;
    sen::sens_t sensor_id = sens_t::NONE;
    sen::data_t data_type = data_t::NONE;

    uint8_t useless_byte;

    MessageInterpreterListener *listener;

    void readDataPackets( uint8_t &bytes_amount );

    void interpretHeader(
        sen::packet_t &type, sen::inst_t &instruction, sen::sens_t &sensor_id, sen::data_t &data_type,
        uint8_t &bytes_amount );

    void clear();

    void run();
    static void staticRun( void *pvParameters );
};

}  // namespace sen

#endif  // R2D2_MESSAGE_INTERPRETER_HPP