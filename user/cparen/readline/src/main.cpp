#include <Arduino.h>
#include "SerialReadLine.hpp"


SerialReadLine rl;

void setup() {
    Serial.begin(115200);
    Serial.print("Arduino CLI (VT100 mode required)\r\n> ");
}

void loop() {
    const char* completedLine = rl.update();
        
    if (completedLine != nullptr) {
        // A line has been completed and returned
        Serial.print("\nCommand received: ");
        Serial.println(completedLine);

        // You would process the command (e.g., check if (command.startsWith("SET")))
        // ...
        
        Serial.print("> ");
    }
}