#include "photo_diode.hpp"
#include "data_receiver.hpp"
#include "message_interpreter.hpp"
#include "data_sender.hpp"
#include "dummy_data.hpp"

#include <unordered_set>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <Arduino.h>

#define FREQUENCY 400

sen::MessageInterpreter interpreter( 32, 1 );
sen::DummyData dummy(&interpreter);

void setup() {
    Serial.begin( 57600 );
}

void loop() {
    delay( 100 );
}