#include "travel_control.hpp"

#define R2D2_DEBUG_ENABLE
#include "r2d2_debug_macros.hpp"

namespace asn {

TravelControl::TravelControl( MotorControl &motorControl, SteerControl &steerControl ) :
    motorControl( motorControl ), steerControl( steerControl ), do_stop( false ) {
    new_dest_queue = xQueueCreate( 5, sizeof( std::array<float, 3> ) );
    cur_queue = xQueueCreate( 5, sizeof( std::array<float, 3> ) );
}

void TravelControl::calculateRotation( const float cur_x, const float cur_z ) {
    float angle = acos(
        ( ( ( cur_x - prev_x ) * ( dest_x - cur_x ) ) + ( ( cur_z - prev_z ) * ( dest_z - cur_z ) ) ) /
        ( sqrt( pow( ( cur_x - prev_x ), 2 ) + pow( ( cur_z - prev_z ), 2 ) ) *
          sqrt( pow( ( dest_x - cur_x ), 2 ) + pow( ( dest_z - cur_z ), 2 ) ) ) );
    angle = angle * ( 180 / ( atan( 1 ) * 4 ) );
    steerControl.setSetpoint( angle );
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
    travel_state_t travel_state = start;
    float new_dest[3];
    float cur[3];
    float cur_x = 0;
    float cur_y = 0;
    float cur_z = 0;

    for ( ;; ) {
        switch ( travel_state ) {
            case start:
                if ( xQueueReceive( cur_queue, cur, 0 ) ) {
                    R2D2_DEBUG_LOG( "STATE START READING CUR QUEUE: %f, %f, %f", cur[0], cur[1], cur[2] );
                    prev_x = cur[0];
                    prev_y = cur[1];
                    prev_z = cur[2];
                    travel_state = read;
                } else if ( do_stop ) {
                    travel_state = stop_travel;
                }
                break;
            case read:
                // if bool stop has been set true, stop state.
                // else if cur_cueue has data, update current position state.
                // else if dest_queue has data, dest state.
                if ( do_stop ) {
                    travel_state = stop_travel;
                } else if ( xQueueReceive( cur_queue, cur, 0 ) ) {
                    travel_state = update_current;
                    // Serial.printf("x =  %f, y = %f, z = %f", cur[0], cur[1], cur[2]);
                } else if ( xQueueReceive( new_dest_queue, new_dest, 0 ) ) {
                    travel_state = new_destination;
                }
                break;

            case stop_travel:
                // Serial.println("stop travel");
                motorControl.move( motorControl.direction_t::STOP );
                steerControl.disable();

                xQueueReset( cur_queue );

                do_stop = false;

                travel_state = read;
                break;

            case update_current:
                R2D2_DEBUG_LOG( "!!!!!!!!!!!TravelControl state update current" );
                cur_x = cur[0];
                cur_y = cur[1];
                cur_z = cur[2];

                // if arrived at x and z.
                if ( dest_x == cur_x && dest_z == cur_z ) {
                    // Serial.println("height time");
                    steerControl.disable();
                    if ( cur_y < dest_y ) {
                        motorControl.move( motorControl.direction_t::UP );
                    } else if ( cur_y > dest_y ) {
                        motorControl.move( motorControl.direction_t::DOWN );
                    } else {
                        motorControl.move( motorControl.direction_t::STOP );
                    }
                    // not arrived, prev same as z. Stuck?
                } else if ( prev_x == cur_x && prev_z == cur_z ) {
                    // stop steering when going backwards.
                    //  Serial.println("back time");
                    steerControl.disable();
                    motorControl.move( motorControl.direction_t::BACKWARD );
                    vTaskDelay( 100 );
                    motorControl.move( motorControl.direction_t::STOP );
                    steerControl.enable();
                } else {
                    // x and y not right, so continue steering.
                    steerControl.enable();
                    calculateRotation( cur_x, cur_z );
                }

                // update prev
                prev_x = cur_x;
                prev_y = cur_y;
                prev_z = cur_z;

                travel_state = read;
                break;

            case new_destination:
                // Serial.println("new dest");
                dest_x = new_dest[0];
                dest_y = new_dest[1];
                dest_z = new_dest[2];
                motorControl.move( motorControl.direction_t::FORWARD );
                vTaskDelay( 100 );
                motorControl.move( motorControl.direction_t::STOP );

                travel_state = read;
                break;

            default:
                break;
        }
    }
}

}  // namespace asn