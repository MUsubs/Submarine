#include "travel_control.hpp"

namespace asn {

TravelControl::TravelControl( MotorControl &motorControl, SteerControl &steerControl ) :
    motorControl( motorControl ), steerControl( steerControl ), do_stop( false ) {
        new_dest_queue = xQueueCreate( 5, sizeof(std::array<float,3>));
        cur_queue = xQueueCreate( 5, sizeof(std::array<float,3>));
}

void TravelControl::calculateRotation( const float cur_x, const float cur_z ) {
    float angle = acos(
        ( ( ( cur_x - prev_x ) * ( dest_x - cur_x ) ) + ( ( cur_z - prev_z ) * ( dest_z - cur_z ) ) ) /
        ( sqrt( pow( ( cur_x - prev_x ), 2 ) + pow( ( cur_z - prev_z ), 2 ) ) *
          sqrt( pow( ( dest_x - cur_x ), 2 ) + pow( ( dest_z - cur_z ), 2 ) ) ) );
    angle = angle * ( 180 / ( atan( 1 ) * 4 ) );
    steerControl.setSetpoint( angle );
    Serial.printf( "Finished calculateRotation, result = %f\n", angle );
}

void TravelControl::stop() {
    do_stop = true;
}

void TravelControl::newDest( float new_dest_x, float new_dest_y, float new_dest_z ) {
    // write to dest queue
    std::array<float,3> dest_array = {new_dest_x, new_dest_y, new_dest_z};
    xQueueSend( new_dest_queue, (void*)&dest_array, 0 );
}

void TravelControl::updateCurPos( float cur_x, float cur_y, float cur_z ) {
    // write to cur queue
    std::array<float,3> cur_array = {cur_x, cur_y, cur_z};
    xQueueSend( cur_queue, (void*)&cur_array, 0 );
}

void TravelControl::main() {
    Serial.println( "start travel" );
    travel_state_t travel_state = read;
    float new_dest[3];
    float cur[3];
    float cur_x = 0;
    float cur_y = 0;
    float cur_z = 0;

    for(;;){
        switch( travel_state ){
            case read:
                // if bool stop has been set true, stop state.
                // else if cur_cueue has data, update current position state.
                // else if dest_queue has data, dest state.
                if ( do_stop ) {
                    travel_state = stop_travel;
                } else if ( xQueueReceive( cur_queue, cur, 0) ){
                    travel_state = update_current;
                    // Serial.printf("x =  %f, y = %f, z = %f", cur[0], cur[1], cur[2]);
                } else if ( xQueueReceive( new_dest_queue, new_dest, 0) ){
                    travel_state = new_destination;
                }
                break;
            
            case stop_travel:
                // Serial.println("stop travel");
                motorControl.move( motorControl.direction_t::STOP );
                steerControl.disable();
            
                do_stop = false;

                travel_state = read;
                break;

            case update_current:
                // Serial.println("update current");
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
                    //stop steering when going backwards.
                        // Serial.println("back time");
                        steerControl.disable();
                        motorControl.move( motorControl.direction_t::BACKWARD );
                        vTaskDelay( 100 );
                        motorControl.move( motorControl.direction_t::STOP );
                        steerControl.enable();
                } else {
                    // x and y not right, so continue steering.
                    steerControl.enable();
                    calculateRotation(cur_x, cur_z);
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
                steerControl.enable();

                travel_state = read;
                break;
            
            default:
                break;
        }
    }
}

}  // namespace asn

// angle = cos^-1((a_x * b_x) + (a_z * b_z) / (sqrt((a_x)^2 + (a_z)^2) *
// sqrt((b_x)^2 + (b_z)^2)))