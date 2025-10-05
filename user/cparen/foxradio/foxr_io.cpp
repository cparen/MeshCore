#include <Arduino.h>   // needed for PlatformIO

namespace foxr
{
  void println(const char* line)
  {
    // TODO: also render to display
    Serial.println(line);
  }

  void readln(char* buf, size_t sz)
  {
    while(1) {
      char ch = Serial.read();
      if (ch == '\n') {
        *buf = 0;
        return;
      }
      if (sz > 1) {
        *(buf++) = ch;
        sz--;
      }
    }
  }
}