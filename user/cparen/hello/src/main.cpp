#include "Arduino.h"


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Hello");
}

void loop() {
  delay(1000);
  Serial.println("tick");
}