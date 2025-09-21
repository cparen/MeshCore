
#include <Arduino.h>
#include "Foxr.h"


namespace foxr
{
  class Timer
  {
  public:
    bool inUse = 0;
    bool repeat = 0;
    TimerPersonality personality = 0;
    int nextMs = 0;
    int periodMs = 0;
    unsigned char user[timer_max_alloc];

    void loop()
    {
      if (inUse && personality && periodMs) {
        int delta = nextMs - millis();
        if (delta < 0)
        {
          nextMs += periodMs;

          personality(this);

          if (!repeat)
          {
            clear();
          }
        }
      }
    }
    void clear()
    {
      // TODO: cleanup any user data in buffer
      (*this) = Timer();
    }
  };

  Timer globalTimerSlots[8];


  // Timer management

  Timer* timer_alloc()
  {
    for (Timer& t : globalTimerSlots)
    {
      if (!t.inUse)
      {
        t.inUse = true;
        return &t;
      }
    }
    return 0;
  }

  void   timer_setpersonality(Timer* timer, TimerPersonality personality)
  {
    timer->personality = personality;
  }

  void*  timer_getctx(Timer* timer)
  {
    &timer->user;
  }
  void   timer_schedule(Timer* timer, int periodMs, bool repeat)
  {
    timer->periodMs = periodMs;
    timer->nextMs = millis() + periodMs;
    timer->repeat = repeat;
  }

  void timer_loop()
  {
    // tick every timer forward
    for (Timer& timer : globalTimerSlots) {
      timer.loop();
    }
  }

  void loop() {
    timer_loop();
  }
}