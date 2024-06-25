#ifndef R2D2_PHOTO_DIODE_HPP
#define R2D2_PHOTO_DIODE_HPP

#include <cstdint>

namespace sen {

class DataReceiver;

/**
 * @class PhotoDiode photo_diode.hpp
 * @brief Class for polling a photo diode (or similar, e.g a TSOP)
 * @details
 * ### Class goals:
 * - Identify signals and their duration from the connected photo diode
 * 
 * - Identify pauses between signals and their duration
 *
 * - Send identified signals and pauses to a DataReceiver listener
 * 
 * @see DataReceiver
 */
class PhotoDiode {
public:
    /**
     * @brief Construct a PhotoDiode object
     *
     * @param pin Pin of the photo diode
     * @param poll_frequency How often per second the pin should be polled
     * @param max_pause_us How long a pause is allowed to go on for before notifying the listener
     * @param priority The priority the task should get
     */
    PhotoDiode( int pin, int poll_frequency, int max_pause_us, int priority );
    /**
     * @brief Set a DataReceiver as listener
     * 
     * @param listener Pointer to the DataReceiver
     * 
     * @see DataReceiver
     */
    void setListener( DataReceiver * listener );
private:
    /**
     * @brief Function containing the main loop
     */
    void run();
    /**
     * @brief Static function that calls PhotoDiode::run, used for FreeRTOS task creation
     * 
     * @param task_parameter Parameter given when creating the task. Should point to the PhotoDiode whose run is to be called
     */
    static void staticRun( void * task_parameter );
    enum { wait_for_pause, wait_for_signal } _state = wait_for_pause;
    int _pin;
    int _wait_us;
    int _max_pause_us;
    DataReceiver * _listener = nullptr;
};

} // namespace sen


#endif // R2D2_IRListener_HPP