#include "TravelControl.hpp"

namespace asn {

TravelControl::TravelControl() {
}

void TravelControl::calculateRotation() {
    float angle = acos( ( prev_x * dest_x ) +
                        ( prev_z * dest_z ) /
                            ( sqrt( ( prev_x ) ^ 2 + ( prev_z ) ^ 2 ) *
                              sqrt( ( dest_x ) ^ 2 + ( dest_z ) ^ 2 ) ) );
    angle = angle * ( 180 / ( atan( 1 ) * 4 ) );
    dummyMotor.adjust( angle );
}

void TravelControl::stop() {
}

void TravelControl::newDest() {
}

void TravelControl::updateCurPos() {
}

}  // namespace asn

// angle = cos^-1((a_x * b_x) + (a_z * b_z) / (sqrt((a_x)^2 + (a_z)^2) *
// sqrt((b_x)^2 + (b_z)^2)))