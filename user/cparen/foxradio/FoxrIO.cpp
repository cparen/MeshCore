#include <Arduino.h>   // needed for PlatformIO

namespace foxr
{
	void println(const char* line)
	{
		Serial.println(line);
	}
}