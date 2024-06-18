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
    std::queue<uint32_t> _measure_buffer;
    DummyDataSender& _data_sender;

    void addMeasure( const uint32_t& measure );
    void transmitMeasures();
    void sendPacket( const String& packet_string );

    String readSerial();

    std::tuple<String*, int> extractCommand( String& input );
};

}  // namespace sen

#endif  // R2D2_SERIAL_CONTROL