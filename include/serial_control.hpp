#ifndef R2D2_SERIAL_CONTROL
#define R2D2_SERIAL_CONTROL

#include <Arduino.h>
#include <FreeRTOS.h>

#include <queue>
#include <tuple>

#include "dummy_data_sender.hpp"

namespace sen {

class SerialControl {
public:
    SerialControl( DummyDataSender& data_sender );

    void run();

private:
    std::queue<float> _measure_buffer;
    DummyDataSender& _data_sender;

    void addMeasure( const float& measure );
    void transmitMeasures();
    void sendPacket( const String& packet_string );

    std::tuple<String*, int> extractCommand( const String& input );
    String readSerial();
};

}  // namespace sen

#endif  // R2D2_SERIAL_CONTROL