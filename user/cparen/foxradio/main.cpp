#include <Arduino.h>

#include <cstdio>
#include <future>

#include "RadioConfig.h"
#include "Foxr.h"

using namespace std;

char buf[100];
char buf2[100];

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

  auto _ = std::async([]() -> foxr::BasicCoroutine {
    char buf2[200];
    co_await foxr::readln(buf, countof(buf));
    sprintf(buf2, "heard: %s", buf);
    foxr::println(buf2);
  });
}

void loop() {
  // inform components of loop call 
  foxr::loop();
}

