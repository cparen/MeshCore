#pragma once


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
  Stopwatch timeInState;
  
  State state;

  void personality(int radioActive, int stateChange);
  void changeState(State newState);

public:
  Esp32LightSleep();
  ~Esp32LightSleep();

  // wake duty cycle out of 100.
  int dutyCycle = 10;
  int enabled = 0;
  int wakeCycleOnActivity = 10000;
  int sleepCnt = 0;


  // call from setup to configure lightsleep
  void setup();

  // call from loop to check for lightsleep conditions
  void loop(int bleActive);

  Stopwatch timeAwake;
  Stopwatch timeAsleep;
};
