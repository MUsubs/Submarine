#ifndef R2D2_MESSAGE_INTERPRETER_HPP
#define R2D2_MESSAGE_INTERPRETER_HPP

#include <FreeRTOS.h>
#include <queue.h>

#include <cstdint>

#include "packet_enums.h"
#include "message_decoder.hpp"

namespace sen {

class MessageInterpreter : public sen::MessageListener {
public:
    MessageInterpreter( int queue_length );
    ~MessageInterpreter();

    void messageReceived( uint8_t msg ) override;
    void messageDone() override;

    void main();

private:
    enum stat_t { IDLE, HEADER, EXECUTE, ACKNOWLEDGE };
    stat_t state = IDLE;
    QueueHandle_t _packets_queue;

    QueueHandle_t _message_done_queue;

    uint16_t readDataPackets( uint8_t &bytes_amount );

    void interpretHeader( sen::packet_t &type, sen::inst_t &instruction,
                          sen::sens_t &sensor_id, sen::data_t &data_type, uint8_t &bytes_amount );
};

}  // namespace sen

#endif  // R2D2_MESSAGE_INTERPRETER_HPP