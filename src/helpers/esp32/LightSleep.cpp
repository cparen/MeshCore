#include "esp_sleep.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include "LightSleep.h"
#include <Arduino.h>



void Stopwatch::restart(){
  tstart = millis();
  trunning = 1;
}
void Stopwatch::start() {
  if (trunning) { return; }
  int now = millis();
  int prev_dur = tend - tstart; 
  tstart = now - prev_dur;
  trunning = 1;
}
void Stopwatch::stop() {
  if (!trunning) { return; }
  tend = millis();
  trunning = 0;
}
int Stopwatch::elapsed() {
  if (trunning) { 
    int now = millis();
    return now - tstart;
  } else {
    return tend - tstart;
  }
}

Esp32LightSleep::Esp32LightSleep() {
  timeInState.restart();
  dutyCycle = LIGHTSLEEP_DEFAULT_DUTY;
}

Esp32LightSleep::~Esp32LightSleep() {
}

void Esp32LightSleep::setup() {
  
  Serial.println("Esp32LightSleep::setup()");

  // Set the timer to wake up the ESP32 from lightsleep
  esp_sleep_enable_timer_wakeup(LIGHTSLEEP_TIME_TO_SLEEP_MS * 1000ULL);
}

void Esp32LightSleep::loop(int radioActive) {
  if (!this->enabled) {
    timeInState.restart();
    return;
  }
  personality(radioActive, 0);
}

const int EnterState = 100;
const int ExitState = -100;

void Esp32LightSleep::changeState(State newState) {
  personality(0, ExitState);
  state = newState;
  timeInState.restart();
  timeAwake.start();
  personality(0, EnterState);
}


void Esp32LightSleep::personality(int radioActive, int stateChange) {
  // short wake interval, dutyCycle/100 compared to LIGHTSLEEP_TIME_TO_SLEEP_MS
  int dozeInterval = LIGHTSLEEP_TIME_TO_SLEEP_MS * dutyCycle / 100;
  if (dozeInterval < 1) {
    dozeInterval = 1;
  }
    
  switch (state + stateChange) {
  case InitState: 
    if (timeInState.elapsed() > 1000) {
      changeState(ActiveState);
      return;
    }
    break;
  case (EnterState + ActiveState):
    Serial.println("lightsleep: awake");
    break;
  case ActiveState: // awake
    if (radioActive) {
      // extend awake interval
      timeInState.restart();
      return;
    }
    if (timeInState.elapsed() > activeStateDuration) {
      changeState(SleepySoonState);
      return;
    }
    break;
  case (EnterState + SleepySoonState):
    Serial.println("lightsleep: sleeping...");
    break;
  case SleepySoonState:
    if (radioActive) {
      changeState(ActiveState);
      return;
    }
    // give time for the message to print
    if (timeInState.elapsed() > 500) {
      changeState(SleepyState);
    }
    break;

  case (SleepyState + EnterState):
    timeAwake.stop();
    timeAsleep.start();
    break;
  case (SleepyState + ExitState):
    timeAwake.start();
    timeAsleep.stop();
    break;
  case SleepyState:
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
      Serial.println("lightsleep: waking on activity");
      changeState(ActiveState);
      return;
    }
    // do some light sleep
    esp_light_sleep_start();
    sleepCnt++;


    if (dozeCount * LIGHTSLEEP_TIME_TO_SLEEP_MS > LIGHTSLEEP_MAX_SLEEP_LENGTH) {
      // Wake for a a longer period occasionally. This way even if we're having trouble
      // detecting wakeable activity, like ble, we'll occasionally have a chance to catch 
      // it. (Really only needed on companion radio).
      Serial.println("lightsleep: max sleep");
      changeState(ActiveState);
      dozeCount = 0;
      return;
    }
    changeState(DozeState);
    break;

  case (DozeState + EnterState):
    dozeCount++;
    break;

  case DozeState:
    if (radioActive) { 
      Serial.println("lightsleep: activity detect");
      changeState(ActiveState);
      return;
    }
    if (timeInState.elapsed() < dozeInterval) {
      return;
    }
    changeState(SleepyState);
    break;
  default:
    break;
  }
}
