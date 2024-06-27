#ifndef R2D2_SERIAL_CONTROL
#define R2D2_SERIAL_CONTROL

#include <Arduino.h>

#include <array>
#include <map>
#include <queue>
#include <tuple>

#include "FreeRTOS.h"
#include "data_transceiver.hpp"
#include "packet_enums.hpp"
#include "task.h"

namespace sen {

/**
 * @class SerialControl serial_control.hpp
 * @brief Class for communicating with NAV python desktop application
 * @details
 * ### Class goals:
 * - Gets commands from NAV, passes them through to DataSender (Check Serial
 * buffer periodically for commands)
 *
 * - Receives sensor data packets from MessageInterpreter and adds to
 * measurement buffer to be read out by NAV
 *
 * - Sends sensor data to NAV over Serial connection on command “TRANSMIT”
 */
class SerialControl {
public:
    /**
     * @brief Construct a new Serial Control object
     *
     * @param data_sender SEN wireless data transmission sender
     */
    SerialControl( DataTransceiver& data_sender, int task_priority );

    /**
     * @brief
     * Activate SerialControl task
     *
     * @details
     * Set active sephomore flag, take instance out of IDLE state, into READING
     * state resuming FreeRTOS task
     */
    void activate();

    /**
     * @brief
     * Deactivate SerialControl task, put instance into IDLE state
     * suspending FreeRTOS task
     */
    void deactiveate();

    /**
     * @brief
     * Clear contents of measurements buffer
     */
    void clearMeasurements();

    /**
     * @brief Add a measurement to measurements buffer
     *
     * @param measure measurement to store in float format
     */
    void addMeasure( float measure );

    /**
     * @brief Get the Measurement Count of measurement buffer
     *
     * @return int measurement count
     */
    int getMeasurementCount();

    /**
     * @brief
     * Received ACK packet from IR
     */
    void receivedACK();

private:
    std::queue<float> _measure_buffer;
    DataTransceiver& _data_sender;
    xTaskHandle _this_task_handle;

    // IDLE is fully suspended, task activates with activate()
    enum state_t { IDLE, READING, SERIAL_TRANSMIT, DATA_SEND, SERIAL_ACK };
    state_t _state;

    /**
     * @brief Main loop of SerialControl for FreeRTOS task.
     *
     * @param pvParameters Parameters to task function
     */
    void run( void* pvParameters );

    /**
     * @details static version of SerialConrol::run, for FreeRTOS task creation
     * in
     *
     * @param pvParameters Parameters to task function
     */
    static void staticRun( void* pvParameters );

    /**
     * @brief Transmit and empty contents of measurements buffer over Serial
     *
     */
    void transmitMeasures();

    /**
     * @brief
     * Transmit ACK packet over Serial
     */
    void transmitACK();

    /**
     * @brief Send packet according to command string
     * @details
     * Interpret command string, generate header and data bytes, and send
     * corresponding data packet with given data_sender; Supports Instruction
     * and Update packets, as well as the "TRANSMIT" command.
     *
     * @param packet_string Full command string as gotten from Serial buffer
     */
    void sendPacket( const String& packet_string );

    /**
     * @brief Extract command and arguments from command string
     *
     * @param input Full command string as gotten from Serial buffer
     * @return std::tuple<String*, int> Command + arguments in array of Strings,
     * and number of items in array
     */
    std::tuple<std::array<String, 10>, int> extractCommand( const String& input );

    /**
     * @brief read Serial buffer with Serial object, return String
     *
     * @return First available string from serial buffer, empty string if no
     * available
     */
    String readSerialString();
};

}  // namespace sen

#endif  // R2D2_SERIAL_CONTROL