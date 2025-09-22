#if FOXR_EMBEDDED
#include <Arduino.h>
#endif

// for placement new
#include <new> 
#include <cstdio>
#include <coroutine>
#include <future>
#include <variant>
#include <chrono>
#include <thread>

#include "RadioConfig.h"
#include "Foxr.h"

using namespace std;

char buf[100];

void setup() {

#if FOXR_EMBEDDED
  Serial.begin(115200);
  delay(1000);
#endif


  foxr::println("Serial Begin!"); 

  auto _ = std::async([]() -> foxr::BasicCoroutine {
    for (int i=0;;++i) {
      sprintf(buf, "Hello, World! [%d]", i);
      foxr::println(buf);

      co_await foxr::later(4000);
    } 
    co_return;
  });
}

void loop() {
  foxr::loop();
}

// for compiling for desktop only
#ifndef FOXR_EMBEDDED
int main()
{
  setup();
  for (;;)
  {
    loop();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}
#endif
