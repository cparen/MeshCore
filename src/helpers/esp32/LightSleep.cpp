#include "esp_sleep.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include "LightSleep.h"
#include <Arduino.h>

// Sleep for 3 second (adjust lower for better stability, e.g., 500ms or 500000ULL microseconds)
#define LIGHTSLEEP_TIME_TO_SLEEP_MS  300
#define LIGHTSLEEP_SLEEPY_WAKE_MS 10
#define LIGHTSLEEP_SLEEPY_BLEWAKE_MS 3000

namespace {

  struct Stopwatch {
    int tstart = 0, tend=0, trunning =0;
    void restart(){
      tstart = millis();
      trunning = 1;
    }
    void start() {
      if (trunning) { return; }
      int now = millis();
      int prev_dur = tend - tstart; 
      tstart = now - prev_dur;
      trunning = 1;
    }
    void stop() {
      if (!trunning) { return; }
      tend = millis();
      trunning = 0;
    }
    int elapsed() {
      if (trunning) { 
        int now = millis();
        return now - tstart;
      } else {
        return tend - tstart;
      }
    }
  };
}


struct Esp32LightSleep::StateMachine {
  struct State {
    virtual void active(StateMachine& sm) {}
    virtual void onEnter(StateMachine& sm) {}
    virtual void onExit(StateMachine& sm) {}
  };

  template <class State>
  State* i() { static State instance; return &instance; }

  struct InitState : State{
    void onEnter(StateMachine& sm) override {
      Serial.println("lightsleep: init");
    }
  };

  template <class State>
  void changeState() {
    if (currentState) {
      currentState->onExit(*this);
    }
    currentState = i<State>();
    timeInState.restart();
    currentState->onEnter(*this);
  }
  
  Stopwatch timeInState;
  State* currentState = 0;
  int radioActive = 0;

  StateMachine() {
    changeState<InitState>();
  }
  void loop() {
    currentState->active(*this);
  }
};

Esp32LightSleep::Esp32LightSleep()
{
  stateMachine = new StateMachine();
}
Esp32LightSleep::~Esp32LightSleep()
{
  delete stateMachine;
}

  void print_wakeup_reason();

void Esp32LightSleep::setup() {
  
  Serial.println("Esp32LightSleep::setup()");

  // Set the timer to wake up the ESP32 from lightsleep
  esp_sleep_enable_timer_wakeup(LIGHTSLEEP_TIME_TO_SLEEP_MS * 1000ULL);
}

void Esp32LightSleep::loop(int radioActive) {
  stateMachine->radioActive = radioActive;
  stateMachine->loop();
  #if 0
  switch (state) {
  case 's': // startup
    if (sw.elapsed() > 10000) {
      state = 'a';
      sw.restart();
      Serial.printf("lightsleep: startup delay elapsed. awake.\r\n");
      return;
    }
    break;
  case '!':
    Serial.printf("lightsleep: radio heard, keeping awake.\r\n");
    sw.restart();
    state = '?';
    break;
  case '?':
    if (sw.elapsed() > 1000) {
      state = 'a';
      sw.restart();
    }
    break;
  case 'a': // awake
    if (radioActive) {
      state = '!';
      break;
    }
    if (sw.elapsed() > 3000) {
      state = '4';
      sw.restart();
      sw_inner.restart();
      Serial.printf("lightsleep: countdown to sleep\r\n");
      return;
    }
    break;
  case '4': 
  case '3': 
  case '2':
  case '1':
    if (radioActive) {
      state = '!';
      break;
    }
    if (sw.elapsed() > 1000) {
      state -= 1;
      sw.restart();
      Serial.printf("lightsleep: %c\r\n", state);
      return;
    }
    break;
  case '0':
    Serial.printf("lightsleep: resting\r\n");
    state = 'r';
    sw.restart();
    sw_inner.restart();
    break;

  case 'r': // rest
    //
    // TODO: bluetooth seems to be able to keep alive as long as we stay 
    // awake long enough to respond. the meshcore/lora on the otherhand only seems
    // to need a few dozen milliseconds. Experiment with long-short-short-...-short wake
    // cycles. Then we can break out of sleep when we see a bluetooth packet, but run 
    // very short duty cycle for the lora processing. 
    //
    // Also, clean up state machine readability.
    //
    if (radioActive) { 
      state = '?';
      Serial.printf("lightsleep: waking, radio activity. slept=%d\r\n", sleepcnt);
      break;
    }
    if (sw.elapsed() > 30000) {
      state = '?';
      Serial.printf("lightsleep: waking after quiet rest. slept=%d\r\n", sleepcnt);
      return;
    }
    int sleepy_wake = LIGHTSLEEP_SLEEPY_WAKE_MS;
    if (sleepcnt % 10 == 0) {
      sleepy_wake = LIGHTSLEEP_SLEEPY_BLEWAKE_MS;
    }
    if (sw_inner.elapsed() < sleepy_wake) {
      // stay awake a bit longer, running at 
      // LIGHTSLEEP_SLEEPY_WAKE_MS / LIGHTSLEEP_TIME_TO_SLEEP_MS dutycyle
      return;
    }
    // do some light sleep
    esp_light_sleep_start();
    sleepcnt++;
    sw_inner.restart();
    break;
  }
  #endif
}
