#include <SPI.h>
#include <LoRa.h>

void setup() {
    Serial.begin(9600);
    while (!Serial);

    if (!LoRa.begin(868E6)) {
        Serial.println("LoRa init failed. Check your connections.");
        while (true);
    }
}

void loop() {
}

