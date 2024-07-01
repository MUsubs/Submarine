#ifndef R2D2_MOTOR_CONTROL_HPP
#define R2D2_MOTOR_CONTROL_HPP

#include <Arduino.h>

#include "FreeRTOS.h"
#include "motor.hpp"
#include "queue.h"

namespace asn {

/**
 * @class Class MotorControl motor_control.hpp
 * @brief Receives commands (directions) to instruct Motor object to control motors.
 */
class MotorControl {
public:
    /**
     * @brief Constructur for MotorControl class.
     *
     * @param pins (uint8_t*) Array with pins for Motor object.
     */
    MotorControl( uint8_t *pins );

    /**
     * @enum direction_t
     * @brief enum with directions to use: LEFT, RIGHT, FORWARD, BACKWARD, UP, DOWN, STOP
     */
    enum direction_t { LEFT, RIGHT, FORWARD, BACKWARD, UP, DOWN, STOP };

    /**
     * @brief Adds direction to queue
     */
    void move( direction_t direction );

    /**
     * @brief Turns on motor according to the given direction.
     *
     * @param direction (direction_t) to indicate in which direction to move.
     *
     */
    void main();

private:
    Motor motor;
    xQueueHandle directions_queue;
};

}  // namespace asn

#endif  // R2D2_MOTOR_CONTROL_HPP
