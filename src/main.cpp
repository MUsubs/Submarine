#include "Arduino.h"

String receivedString = ""; // A string to hold incoming data
bool stringComplete = false; // Whether the string is complete

float x = 0.0;
float y = 1.0;
float z = 1.0;


// MAKE THIS WORK
void parseCoordinates(String str) {
    int xIndex = str.indexOf('X');
    int yIndex = str.indexOf('Y');
    int zIndex = str.indexOf('Z');
    Serial.print(xIndex);
    Serial.print(yIndex);
    Serial.print(zIndex);
    if (xIndex != -1 && yIndex != -1 && zIndex != -1) {
        String xStr = str.substring(xIndex + 1, yIndex);
        String yStr = str.substring(yIndex + 1, zIndex);
        String zStr = str.substring(zIndex + 1);

        x = xStr.toFloat();
        y = yStr.toFloat();
        z = zStr.toFloat();
    }
}

void setup() {
    Serial.begin(9600);
    delay(2000);
}

void loop() {
    // Read data from the serial port
    while (Serial.available()) {
        
        char inChar = Serial.read();
        Serial.print(inChar);
        receivedString = String(inChar);
        Serial.print(receivedString);
        // If the incoming character is a newline, the string is complete
        if (inChar == '\n') {
            stringComplete = true;
            break;
        }
    }

    // Check if data has been received
    if (receivedString != "") {
        parseCoordinates(receivedString);
        // Print the parsed coordinates
        Serial.print("X=");
        Serial.print(x);
        Serial.print(" Y=");
        Serial.print(y);
        Serial.print(" Z=");
        Serial.println(z);

        // Clear the string for the next input
        receivedString = "";
        stringComplete = false;
    }
}
