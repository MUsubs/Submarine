#include "dummy_data.hpp"

namespace sen {

DummyData::DummyData( MessageInterpreter* message_interpreter ) :
    message_interpreter( message_interpreter ) {
    xTaskCreate( staticRun, "Dummy_data", 4000, (void*)this, 1,
                 &_this_task_handle );
}

void DummyData::run() {
    for ( ;; ) {
        switch ( _state ) {
            case IDLE:
                vTaskDelay( 15000 );
                _state = SEND;
                break;

            case SEND:
                uint8_t header = 0b11010010;
                message_interpreter->byteReceived( header );
                uint8_t data = 0b11111111;
                for ( unsigned int i = 0; i < 2; i++ ) {
                    message_interpreter->byteReceived( data );
                }

                message_interpreter->messageDone();

                _state = IDLE;
                break;
        }
    }
}

void DummyData::staticRun( void* pvParameters ) {
    DummyData* dummy_data = reinterpret_cast<DummyData*>( pvParameters );
    dummy_data->run();
    vTaskDelete( dummy_data->_this_task_handle );
}

}  // namespace sen