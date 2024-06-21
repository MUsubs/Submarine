#ifndef R2D2_PHOTO_DIODE_HPP
#define R2D2_PHOTO_DIODE_HPP

#include <cstdint>

namespace sen {

class DataReceiver;

class PhotoDiode {
public:
    PhotoDiode( int pin, int poll_frequency, int max_pause_us, int priority );
    void setListener( DataReceiver * listener );
    void run();
    static void staticRun( void * task_parameter );
private:
    enum { wait_for_pause, wait_for_signal } _state = wait_for_pause;
    int _pin;
    int _wait_us;
    int _max_pause_us;
    DataReceiver * _listener = nullptr;
};

} // namespace sen


#endif // R2D2_IRListener_HPP