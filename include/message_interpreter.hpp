/// @file
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

/**
 * @class MessageInterpreter message_interpreter.hpp
 * "include/message_interpreter.hpp"
 * @brief
 * Class MessageInterpreter is a class with functions for interpreting incoming
 * messages
 * @details
 * Goals:
 * 1. Receive bytes from DataTransceiver instancce
 * 2. Interpret packet type and data once message is finished
 * 3. Inform listeners of received packet with associated data
 */
class MessageInterpreter {
public:
    /**
     * @brief Construct a new Message Interpreter object
     * Initialises the MessageInterpreter task and the corresponding
     * queues
     * 
     * @param queue_length
     * @param task_priority
     */
    MessageInterpreter( int queue_length, int task_priority );

    /**
     * @brief Destroy the Message Interpreter object
     * Deletes task and queues
     */
    ~MessageInterpreter();

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
     * @brief Enqueues a received byte for further processing.
     *
     * This function takes a single byte received as a message and enqueues it
     * into the packets queue. This allows the byte to be processed later as
     * part of a complete message.
     *
     * @param[in] msg  The byte received to be enqueued.
     *
     * @return void
     */
    void byteReceived( uint8_t msg );

    /**
     * @brief Signals that a message has been completely received and processed.
     *
     * This function enqueues a signal byte (value of 1) into the message done
     * queue. This signal indicates that the current message has been fully
     * received and is ready for further processing.
     *
     * @return void
     */
    void messageDone();

    /**
     * @brief Set the Listener object
     *
     * @param set_listener MessageInterpreterListener pointer
     */
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

    /**
     * @brief Reads data packets from the message queue.
     *
     * This function reads a specified number of data bytes from the message
     * queue and stores them in the `data_array`. It ensures that the number of
     * bytes to be read does not exceed the size of `data_array`.
     *
     * @param[in,out] bytes_amount  Specifies the number of data bytes to be
     * read.
     *
     * @return void
     * @internal
     */

    void readDataPackets( uint8_t &bytes_amount );

    /**
     * @brief Reads the header byte of the message.
     *
     * This function interprets the header byte of a message and extracts
     * various components such as the message type, instruction, sensor ID, data
     * type, and the number of data bytes.
     *
     * @param[out] type          Identifies the type of the received message
     *                           (update packet, instruction, data packet,
     * etc.).
     * @param[out] instruction   Identifies the specific instruction that needs
     *                           to be executed.
     * @param[out] sensor_id     Indicates whether the data belongs to a sensor
     *                           measurement and to which sensor in specific.
     * @param[out] data_type     Identifies what kind of data is being received
     *                           (sub coordination).
     * @param[out] bytes_amount  Indicates how many data bytes belong to the
     *                           message and should be read.
     * @return void
     * @internal
     */
    void interpretHeader(
        sen::packet_t &type, sen::inst_t &instruction, sen::sens_t &sensor_id,
        sen::data_t &data_type, uint8_t &bytes_amount );

    /**
     * @brief Resets the message interpreter to its initial state.
     *
     * This function clears the current message data by resetting all relevant
     * member variables to their default or initial values. It ensures that any
     * previous message data is discarded, and the interpreter is ready to
     * process a new message.
     *
     * @internal
     */
    void clear();

    void run();
    static void staticRun( void *pvParameters );
};

}  // namespace sen

#endif  // R2D2_MESSAGE_INTERPRETER_HPP