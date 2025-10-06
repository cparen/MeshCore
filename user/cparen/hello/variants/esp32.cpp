#include <Arduino.h>   // needed for PlatformIO

void on_start();
void setup()
{

  // init serial
  Serial.begin(115200);
  delay(1000);

  on_start();
}

void loop()
{
}

namespace h
{
  void println(const char* s) 
  {
    // on native, use puts to console.
    Serial.println(s);
  }
}
