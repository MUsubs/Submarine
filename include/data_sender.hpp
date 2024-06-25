#ifndef R2D2_DATA_SENDER_HPP
#define R2D2_DATA_SENDER_HPP

#include <FreeRTOS.h>
#include <queue.h>
#include <vector>
#include <semphr.h>
#include <Arduino.h>

#include "packet_enums.h"

namespace sen {

/**
 * @class DataSender data_sender.hpp
 * @brief Class for sending data over IR
 */
class DataSender {
public:
    /**
     * @brief Construct a DataSender object
     *
     * @param led_pin Pin of the IR-led
     * @param frequency The frequency (bits/sec, roughly) at which data should be send
     * @param byte_queue_length Length of the queue used to store bytes to send
     * @param task_priority The priority the task should get
     */
    DataSender( int led_pin, int frequency, int byte_queue_length, int task_priority );
    /**
     * @brief Add bytes to the byte queue to be send.
     *
     * @param bytes bytes to send
     */
    void sendBytes( const std::vector<uint8_t>& bytes );
    /**
     * @brief Generate an instruction header byte
     *
     * @param inst which kind of instruction
     * @param n_bytes size of the data body
     * 
     * @return A byte to be used as an header in communication.
     */
    uint8_t generateInstructionHeader( inst_t inst, uint8_t n_bytes );
    /**
     * @brief Generate an update header byte
     *
     * @param data_id which kind of data to update
     * @param n_bytes size of the data body
     * 
     * @return A byte to be used as an header in communication.
     */
    uint8_t generateUpdateHeader( data_t data_id, uint8_t n_bytes );
    /**
     * @brief Generate a sensor header byte
     *
     * @param sensor which kind of sensor to read
     * @param n_bytes size of the data body
     * 
     * @return A byte to be used as an header in communication.
     */
    uint8_t generateSensorHeader( sens_t sensor, uint8_t n_bytes );
    // ~DataSender();
private:
    /**
     * @brief Function containing the main loop
     */
    void run();
    /**
     * @brief Static function that calls DataSender::run, used for FreeRTOS task creation
     * 
     * @param task_parameter Parameter given when creating the task. Should point to the PhotoDiode whose run is to be called
     */
    static void staticRun( void * task_parameter );
    QueueHandle_t _byte_queue;
    int _unit_us;
    int _led_pin;
};

} // namespace sen

#endif // R2D2_DATA_SENDER_HPP