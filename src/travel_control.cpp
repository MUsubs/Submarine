#include "travel_control.hpp"

#define R2D2_DEBUG_ENABLE
#include "r2d2_debug_macros.hpp"

namespace asn {

TravelControl::TravelControl( MotorControl &motor_control, SteerControl &steer_control ) :
    motor_control( motor_control ), steer_control( steer_control ), do_stop( false ) {
    new_dest_queue = xQueueCreate( 5, sizeof( std::array<float, 3> ) );
    cur_queue = xQueueCreate( 5, sizeof( std::array<float, 3> ) );
}

void TravelControl::calculateRotation( const float cur_x, const float cur_z ) {
    float angle = acos(
        ( ( ( cur_x - prev_x ) * ( dest_x - cur_x ) ) + ( ( cur_z - prev_z ) * ( dest_z - cur_z ) ) ) /
        ( sqrt( pow( ( cur_x - prev_x ), 2 ) + pow( ( cur_z - prev_z ), 2 ) ) *
          sqrt( pow( ( dest_x - cur_x ), 2 ) + pow( ( dest_z - cur_z ), 2 ) ) ) );
    angle = angle * ( 180 / ( atan( 1 ) * 4 ) );
    steer_control.setSetpoint( angle );
    R2D2_DEBUG_LOG( "Finished calculateRotation, result = %f\n", angle );
}

void TravelControl::stop() {
    R2D2_DEBUG_LOG( "TravelControl STOP" );
    do_stop = true;
}

void TravelControl::newDest( float new_dest_x, float new_dest_y, float new_dest_z ) {
    R2D2_DEBUG_LOG( "TravelControl NEW_POS: %f, %f, %f", new_dest_x, new_dest_y, new_dest_z );
    // write to dest queue
    std::array<float, 3> dest_array = { new_dest_x, new_dest_y, new_dest_z };
    xQueueSend( new_dest_queue, (void *)&dest_array, 0 );
}

void TravelControl::updateCurPos( float cur_x, float cur_y, float cur_z ) {
    R2D2_DEBUG_LOG( "TravelControl UPDATE: %f, %f, %f", cur_x, cur_y, cur_z );
    // write to cur queue
    std::array<float, 3> cur_array = { cur_x, cur_y, cur_z };
    xQueueSend( cur_queue, (void *)&cur_array, 0 );
}

void TravelControl::main() {
    R2D2_DEBUG_LOG( "TravelControl start main" );
    travel_state_t travel_state = START;
    float new_dest[3];
    float cur[3];
    float cur_x = 0;
    float cur_y = 0;
    float cur_z = 0;

    for ( ;; ) {
        switch ( travel_state ) {
            case START:
                if ( xQueueReceive( cur_queue, cur, 0 ) ) {
                    R2D2_DEBUG_LOG( "STATE START READING CUR QUEUE: %f, %f, %f", cur[0], cur[1], cur[2] );
                    prev_x = cur[0];
                    prev_y = cur[1];
                    prev_z = cur[2];
                    travel_state = READ;
                } else if ( do_stop ) {
                    travel_state = STOP_TRAVEL;
                }
                break;
            case READ:
                // if bool stop has been set true, stop state.
                // else if cur_cueue has data, update current position state.
                // else if dest_queue has data, dest state.
                if ( do_stop ) {
                    travel_state = STOP_TRAVEL;
                } else if ( xQueueReceive( cur_queue, cur, 0 ) ) {
                    travel_state = UPDATE_CURRENT;
                    // Serial.printf("x =  %f, y = %f, z = %f", cur[0], cur[1], cur[2]);
                } else if ( xQueueReceive( new_dest_queue, new_dest, 0 ) ) {
                    travel_state = NEW_DESTINATION;
                }
                break;

            case STOP_TRAVEL:
                // Serial.println("stop travel");
                motor_control.move( motor_control.direction_t::STOP );
                steer_control.disable();

                xQueueReset( cur_queue );

                do_stop = false;

                travel_state = READ;
                break;

            case UPDATE_CURRENT:
                R2D2_DEBUG_LOG( "!!!!!!!!!!!TravelControl state update current" );
                cur_x = cur[0];
                cur_y = cur[1];
                cur_z = cur[2];

                // if arrived at x and z.
                if ( dest_x == cur_x && dest_z == cur_z ) {
                    // Serial.println("height time");
                    steer_control.disable();
                    if ( cur_y < dest_y ) {
                        motor_control.move( motor_control.direction_t::UP );
                    } else if ( cur_y > dest_y ) {
                        motor_control.move( motor_control.direction_t::DOWN );
                    } else {
                        motor_control.move( motor_control.direction_t::STOP );
                    }
                    // not arrived, prev same as z. Stuck?
                } else if ( prev_x == cur_x && prev_z == cur_z ) {
                    // stop steering when going backwards.
                    //  Serial.println("back time");
                    steer_control.disable();
                    motor_control.move( motor_control.direction_t::BACKWARD );
                    vTaskDelay( 100 );
                    motor_control.move( motor_control.direction_t::STOP );
                    steer_control.enable();
                } else {
                    // x and y not right, so continue steering.
                    steer_control.enable();
                    calculateRotation( cur_x, cur_z );
                }

                // update prev
                prev_x = cur_x;
                prev_y = cur_y;
                prev_z = cur_z;

                travel_state = READ;
                break;

            case NEW_DESTINATION:
                // Serial.println("new dest");
                dest_x = new_dest[0];
                dest_y = new_dest[1];
                dest_z = new_dest[2];
                motor_control.move( motor_control.direction_t::FORWARD );
                vTaskDelay( 100 );
                motor_control.move( motor_control.direction_t::STOP );

                travel_state = READ;
                break;

            default:
                break;
        }
    }
}

}  // namespace asn