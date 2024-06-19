#include "serial_control.hpp"
#include "dummy_data_sender.hpp"

sen::DummyDataSender dummy_sender{LED_BUILTIN, 30};
sen::SerialControl serial_control{dummy_sender, 1};

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
    // while(!Serial){
    //     digitalWrite(LED_BUILTIN, HIGH);
    // }
    // digitalWrite(LED_BUILTIN, LOW);
    // vTaskDelay(2000);
    serial_control.activate();
}

void loop() {
    taskYIELD();
}