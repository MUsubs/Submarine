#include "dummy_travel_control.hpp"

namespace sen {

DummyTravelControl::DummyTravelControl() {
}

void DummyTravelControl::stop() {
    Serial.println("==INFO== DUMMY TRAVEL CONTROL - STOP");
}

void DummyTravelControl::newDest(float x, float y, float z) {
    Serial.printf("==INFO== DUMMY TRAVEL CONTROL - NEW DEST %f, %f, %f\n", x, y, z);
}

void DummyTravelControl::updateCurPos( float x, float y, float z ) {
    Serial.printf("==INFO== DUMMY TRAVEL CONTROL - UPDATE CUR POS %f, %f, %f\n", x, y, z);
}

}  // namespace sen