#ifndef R2D2_MESSAGE_INTERPRETER_LISTENER_HPP
#define R2D2_MESSAGE_INTERPRETER_LISTENER_HPP

#include "packet_enums.hpp"

namespace sen {

class MessageInterpreterListener {
public:
    /**
     * @name receivedINST Functions
     * @{
     */

    /**
     * @fn void receivedINST( InstPacket_t& inst_p )
     * @brief Notify of received Instruction
     *
     * @param inst_p Instruction Packet with type and data
     */
    virtual void receivedINST( InstPacket_t& inst_p ) {};

    /**
     * @overload
     *
     * @param inst_type Type of instruction
     * @param data Data associated with instruction arguments
     */
    virtual void receivedINST( inst_t inst_type, std::array<uint8_t, 3>& data ) {};

    /**
     * @overload
     * @brief Single Byte instruction variant
     *
     * @param inst_type Type of instruction
     */
    virtual void receivedINST( inst_t inst_type ) {};

    /** @} */

    /**
     * @name receivedUPDATE Functions
     * @{
     */

    /**
     * @fn void receivedUPDATE( UpdatePacket_t& update_p )
     * @brief Notify of received Update
     *
     * @param update_p Update Packet with type and data
     */
    virtual void receivedUPDATE( UpdatePacket_t& update_p ) {};

    /**
     * @overload
     *
     * @param data_type Data field to update
     * @param data Data associated with update arguments
     */
    virtual void receivedUPDATE( data_t data_type, std::array<uint8_t, 3>& data ) {};

    /** @} */

    /**
     * @brief Notify of received Sens packet
     *
     * @param sensor Sensor field
     * @param data Data from sensor
     */
    virtual void receivedSENS( sens_t sensor, float data ) {};
};

}  // namespace sen
#endif  // R2D2_MESSAGE_INTERPRETER_LISTENER_HPP