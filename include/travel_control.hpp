#ifndef R2D2_TRAVELCONTROL_HPP
#define R2D2_TRAVELCONTROL_HPP

#include <array>
#include <cmath>

#include "motor_control.hpp"
#include "steer_control.hpp"

namespace asn {

/**
 * @class Class travel_control.hpp
 * @brief A control class thaat controls the submarine travel.
 */
class TravelControl {
public:
    TravelControl( MotorControl &motor_control, SteerControl &steer_control );
    /**
     * @brief Calculates the needed rotation.
     * @details Calculates the rotation in degrees that the submarine
     * must make in order to get on course to the destination.
     * @param cur_x The current x-coordinate of the submarine.
     * @param cur_z The current z-coordinate of the submarine.
     */
    void calculateRotation( const float cur_x, const float cur_z );
    /**
     * @brief Stops the submarine.
     */
    void stop();
    /**
     * @brief Sends a new destination to the submarine.
     * @param dest_x The x-coordinate of the new destination.
     * @param dest_y The y-coordinate of the new destination.
     * @param dest_z The z-coordinate of the new destination.
     */
    void newDest( const float dest_x, const float dest_y, const float dest_z );
    /**
     * @brief Updates the current position.
     * @param cur_x The current x-coordinate.
     * @param cur_y The current y-coordinate.
     * @param cur_z The current z-coordinate.
     */
    void updateCurPos( const float cur_x, const float cur_y, const float cur_z );
    void main();

private:
    MotorControl &motor_control;
    SteerControl &steer_control;
    float dest_x = 0;
    float dest_y = 0;
    float dest_z = 0;
    float prev_x = 0;
    float prev_y = 0;
    float prev_z = 0;

    xQueueHandle new_dest_queue;
    xQueueHandle cur_queue;
    bool do_stop;

    enum travel_state_t { START, READ, STOP_TRAVEL, UPDATE_CURRENT, NEW_DESTINATION };
};

}  // namespace asn
#endif  // R2D2_TRAVELCONTROL_HPP