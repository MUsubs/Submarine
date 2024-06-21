#ifndef R2D2_DATA_SENDER_HPP
#define R2D2_DATA_SENDER_HPP

#include <FreeRTOS.h>
#include <queue.h>
#include <vector>
#include <cstdint>

#include "packet_enums.h"

namespace sen {

class DataSender {
public:
    DataSender( int led_pin, int frequency, int byte_queue_length, int task_priority );
    void run();
    static void staticRun( void * task_parameter );
    void sendBytes( const std::vector<uint8_t>& bytes );
    uint8_t generateInstructionHeader( inst_t inst, uint8_t n_bytes );
    uint8_t generateUpdateHeader( data_t data_id, uint8_t n_bytes );
    uint8_t generateSensorHeader( sens_t sensor, uint8_t n_bytes );
    // ~DataSender();
private:
    QueueHandle_t _byte_queue;
    int _unit_us;
    int _led_pin;
};

} // namespace sen

#endif // R2D2_DATA_SENDER_HPP