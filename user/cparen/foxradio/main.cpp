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
  // init searial
  Serial.begin(115200);
  delay(1000);
  foxr::println("Serial Begin!"); 

  // start background hello world heartbeat
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
  // inform components of loop call 
  foxr::loop();
}

