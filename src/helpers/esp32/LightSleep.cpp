#include "esp_sleep.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include "LightSleep.h"
#include <Arduino.h>

// Sleep for 3 second (adjust lower for better stability, e.g., 500ms or 500000ULL microseconds)
#define LIGHTSLEEP_TIME_TO_SLEEP_MS  500
#define LIGHTSLEEP_SLEEPY_WAKE_MS 50
#define LIGHTSLEEP_SLEEPY_BLEWAKE_MS 3000

namespace esp32 {
  int lightsleep_waketime = 0;
  int sleepcnt = 0;

  struct stopwatch {
    int start = 0, end=0, running =0;
    void restart(){
      start = millis();
      running = 1;
    }
    void start() {
      if (running) { return; }
      int now = millis();
      int prev_dur = end - start; 
      start = now - prev_dur;
      running = 1;
    }
    void stop() {
      if (!running) { return; }
      end = millis();
      running = 0;
    }
    int elapsed() {
      if (running) { 
        int now = millis();
        return now - start;
      } else {
        return end - start;
      }
    }
  };

  class LightSleepStateMachine {
    enum States {
      Init,
      Active,

    };
    enum StateActivity {
      OnEnter,
      OnActive,
      OnIdle
    };
    void personality(StateActivity activity, State state) {
      switch () {
      
      }
    }
    
    stopwatch timeInState;
    State currentState;
  public:
    LightSleepStateMachine() {
      enter_state(STARTUP);
    }
    void loop() {
      personality(OnActive, currentState);
    }
  };

  char state = 's';
  LightSleepStateMachine stateMachine;

  void print_wakeup_reason();

  void lightsleep_setup() {
    
    Serial.println("lightsleep_setup()");
    state = 's';
    sw.restart();

    // Set the timer to wake up the ESP32 from lightsleep
    esp_sleep_enable_timer_wakeup(LIGHTSLEEP_TIME_TO_SLEEP_MS * 1000ULL);
  }

  void lightsleep_loop(int radioActive) {
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
  }
}
