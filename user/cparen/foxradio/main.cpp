#if FOXR_EMBEDDED
#include <Arduino.h>
#endif

#include <coroutine>
// for placement new
#include <new> 
//#include <generator>
#include <cstdio>

#include "RadioConfig.h"
#include "Foxr.h"

using namespace std;

#if FOXR_EMBEDDED
template <class T>
class generator
{

};
#endif

generator<int> g;

generator<int> fib()
{
  int a=0, b=1;
  while (true) {
    co_yield a;
    b += a;
    a = b - a; 
  }
}

char buf[100];

void setup() {
#if FOXR_EMBEDDED
  Serial.begin(115200);
  delay(1000);
#endif


  forx::println("Serial Begin!"); 

  foxr::periodic(5000, []{
    forx::println("Hello, World!"); 

    int a = g();
    sprintf(buf, "fib -> %d", a)
    Serial.println(buf); 
  });
}

void loop() {
  foxr::loop();
  delay(1);
}
