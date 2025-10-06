#include <cstdio>
#include <ccc_coro.h>
#ifdef ARDUINO
#include <Arduino.h>
#endif

void println(const char* msg);

void user()
{
  char buf[80];

  println("Hello, world!");
  sprintf(buf, "%d + %d = %d", 2, 3, ccc::add(2,3));
  println(buf);
}

#ifdef ARDUINO
void setup()
{
  // init serial
  Serial.begin(115200);
  delay(1000);

  user();
}
void loop()
{

}
void println(const char* msg)
{
  Serial.println(msg);
}
#else
int main()
{
  user();
}
void println(const char* msg)
{
  printf("%s\n", msg);
}
#endif