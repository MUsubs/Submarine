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

/**
 * @class DataTransceiver data_transceiver.hpp "include/data_transceiver.hpp"
 * @brief Data Transceiver with LoRa (SX1276)
 * @details
 * Goals:
 * 1. Periodically receive messages from SX1276 LoRa radio
 * 2. Pass received bytes through to MessageInterpreter instance
 * 3. Send LoRa messages on command
 */
class DataTransceiver {
public:
    /**
     * @brief Construct a new Data Transceiver object
     * @details
     * Construct Data Transceiver object, and initialize SX1276 LoRa module
     *
     * @param nss_pin Chip Select pin
     * @param rst_pin Reste pin
     * @param dio0_pin Data0 / DIO0 pin
     * @param is_sub Current instance is in Submarine
     * @param message_interpreter Reference to MessageInterpreter instance
     * @param task_priority FreeRTOS priority
     */
    DataTransceiver(
        int nss_pin, int rst_pin, int dio0_pin, bool is_sub, MessageInterpreter& message_interpreter,
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
     * @brief Send bytes over LoRa radio
     *
     * @param bytes Bytes to send
     */
    void sendBytes( std::vector<uint8_t>& bytes );

    /**
     * @brief Add bytes to the byte queue to be sent.
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
