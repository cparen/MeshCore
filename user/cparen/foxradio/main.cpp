#include <Arduino.h>   // needed for PlatformIO

#include "RadioConfig.h"
#include "Foxr.h"

void setup() {
  Serial.begin(115200);
  delay(1000);


  Serial.println("Serial Begin!"); 

  foxr::periodic(5000, []{
    Serial.println("Hello, World!"); 
  });
}

void loop() {
  foxr::loop();
  delay(1);
}
