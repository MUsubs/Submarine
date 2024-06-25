#ifndef R2D2_DUMMY_CONTROL_HPP
#define R2D2_DUMMY_CONTROL_HPP

#include <Arduino.h>
#include "travel_control.hpp"

namespace asn {

class DummyControl {
public:
    DummyControl( TravelControl& travelControl );
    /**
     * @brief A dummy function to test the submarine's travel control.
     */
    void main();

private:
    TravelControl& travelControl;
};

}  // namespace asn

#endif  // R2D2_DUMMY_CONTROL_HPP