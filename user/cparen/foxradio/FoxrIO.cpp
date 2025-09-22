#include <Arduino.h>   // needed for PlatformIO

namespace foxr
{
  void println(const char* line)
  {
    // TODO: also render to display
    Serial.println(line);
  }
}