#pragma once

// Sleep interval (adjust lower for better stability, e.g., 5ms)
#define LIGHTSLEEP_TIME_TO_SLEEP_MS  200
#define LIGHTSLEEP_DEFAULT_DUTY 5
#define LIGHTSLEEP_DEFAULT_ACTIVE_STATE_DUR_MS ( 3 * 1000)
#if defined(ROLE_REPEATER)
#define LIGHTSLEEP_MAX_SLEEP_LENGTH (60 * 1000)
#else
// wake up more frequently for companion, give bluetooth chance to connect
#define LIGHTSLEEP_MAX_SLEEP_LENGTH (9 * 1000)
#endif

struct Stopwatch {
  int tstart = 0, tend=0, trunning =0;
  void restart();
  void start();
  void stop();
  int elapsed();
};

class Esp32LightSleep
{
  enum State {
    InitState,
    ActiveState,
    SleepySoonState,
    SleepyState,
    DozeState
  };
  
  State state;
  Stopwatch timeInState;
  int dozeCount;

  void personality(int radioActive, int stateChange);
  void changeState(State newState);

public:
  Esp32LightSleep();
  ~Esp32LightSleep();

  // wake duty cycle out of 100.
  int enabled = 0;
  int dutyCycle = LIGHTSLEEP_DEFAULT_DUTY;
  int activeStateDuration = LIGHTSLEEP_DEFAULT_ACTIVE_STATE_DUR_MS;
  int sleepCnt = 0;


  // call from setup to configure lightsleep
  void setup();

  // call from loop to check for lightsleep conditions
  void loop(int bleActive);

  // user commands to configure lightsleep
  bool command(const char* input, char* reply);

  Stopwatch timeAwake;
  Stopwatch timeAsleep;
};
