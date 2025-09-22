#ifdef FOXR_EMBEDDED
#include <Arduino.h>   // needed for PlatformIO
#else
#include <cstdio>
#endif

namespace foxr
{
#ifdef FOXR_EMBEDDED
  void println(const char* line)
  {
    Serial.println(line);
  }
#else
  void println(const char* line)
  {
    printf("%s\n", line);
  }
#endif

}