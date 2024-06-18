#ifndef R2D2_SERIAL_CONTROL
#define R2D2_SERIAL_CONTROL

#include <Arduino.h>
#include "FreeRTOS.h"
#include "task.h"

#include <queue>
#include <tuple>

#include "dummy_data_sender.hpp"

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
    SerialControl( DummyDataSender& data_sender );

    /**
     * @brief Main loop of SerialControl for FreeRTOS task.
     */
    void run();

private:
    std::queue<float> _measure_buffer;
    DummyDataSender& _data_sender;

    enum class state_t {IDLE, READING, TRANSMIT}

    /**
     * @brief Add a measurement to measurements buffer
     *
     * @param measure measurement to store in float format
     */
    void addMeasure( const float& measure );

    /**
     * @brief Transmit and empty contents of measurements buffer
     * 
     */
    void transmitMeasures();

    /**
     * @brief Send packet according to command string
     * @details
     * Interpret command string, generate header and data bytes, and send corresponding data packet with given data_sender; 
     * Supports Instruction and Update packets, as well as the "TRANSMIT" command.
     * 
     * @param packet_string Full command string as gotten from Serial buffer
     */
    void sendPacket( const String& packet_string );

    /**
     * @brief Extract command and arguments from command string
     * 
     * @param input Full command string as gotten from Serial buffer
     * @return std::tuple<String*, int> Command + arguments in array of Strings, and number of items in array
     */
    std::tuple<String*, int> extractCommand( const String& input );
    String readSerial();
};

}  // namespace sen

#endif  // R2D2_SERIAL_CONTROL