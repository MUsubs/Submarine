#include "dummyControl.hpp"

namespace asn {

DummyControl::DummyControl( TravelControl& travelControl ) : travelControl( travelControl ) {
}

void DummyControl::main() {
    Serial.println( "start dummy" );
    
    // for(;;){
    travelControl.updateCurPos( 0.0, 0.0, 0.0 );
    vTaskDelay( 1000 );
    travelControl.newDest( 1.0, 1.0, 1.0 );
    vTaskDelay( 1000 );
    travelControl.updateCurPos(0.5, 0.0, 0.0);
    // vTaskDelay( 1000 );
    // travelControl.updateCurPos( 0.5, 0.0, 0.0 );
    // vTaskDelay ( 1000 );
    // travelControl.updateCurPos( 0.5, 0.0, 0.3);
    // vTaskDelay ( 1000 );
    // travelControl.updateCurPos(1.0, 0.0, 1.0);
    // vTaskDelay(1000);
    // travelControl.updateCurPos(1.0, 1.0, 1.0);
    // vTaskDelay(1000);
    // travelControl.stop();

    // travelControl.updateCurPos( 0.4, 0.1, 0.5);
    // vTaskDelay( 1000);
    // travelControl.updateCurPos( 0.7, 0.1, 0.7);
    // }

}

}  // namespace asn
