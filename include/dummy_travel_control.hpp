#ifndef R2D2_DUMMY_TRAVEL_CONTROL_HPP
#define R2D2_DUMMY_TRAVEL_CONTROL_HPP

#include <Arduino.h>

namespace sen {

class DummyTravelControl {
public:
    DummyTravelControl();
    void stop();
    void newDest( float x, float y, float z );
    void updateCurPos( float x, float y, float z );
};

}  // namespace sen
#endif  // R2D2_DUMMY_TRAVEL_CONTROL_HPP