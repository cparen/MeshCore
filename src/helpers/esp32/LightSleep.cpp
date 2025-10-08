#include "esp_sleep.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include "LightSleep.h"
#include <Arduino.h>

// Sleep for 3 second (adjust lower for better stability, e.g., 500ms or 500000ULL microseconds)
#define LIGHTSLEEP_TIME_TO_SLEEP_MS  3000
#define LIGHTSLEEP_SLEEPY_WAKE_MS 300

namespace esp32 {
  int lightsleep_waketime = 0;
  int sleepcnt = 0;

  struct stopwatch {
    int start = 0;
    void restart() { 
      start = millis();
    }
    int elapsed() { 
      int now = millis();
      return (now - start);
    }
  };

  char state = 's';
  stopwatch sw, sw_inner;

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
      if (sw.elapsed() > 10000) {
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
      if (sw.elapsed() > 1000) {
        state -= 1;
        sw.restart();
        Serial.printf("lightsleep: %d\r\n", state);
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
      if (radioActive) { 
        state = '?';
        Serial.printf("lightsleep: waking, radio activity. slept=%d\r\n", sleepcnt);
        break;
      }
      if (sw.elapsed() > 20000) {
        state = '?';
        Serial.printf("lightsleep: waking after quiet rest. slept=%d\r\n", sleepcnt);
        return;
      }
      if (sw_inner.elapsed() < LIGHTSLEEP_SLEEPY_WAKE_MS) {
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
