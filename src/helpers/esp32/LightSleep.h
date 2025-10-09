#ifndef LIGHTSLEEP_H
#define LIGHTSLEEP_H



class Esp32LightSleep
{
  struct StateMachine;
  StateMachine* stateMachine;

public:
  Esp32LightSleep();
  ~Esp32LightSleep();

  // wake duty cycle out of 1000.
  int dutyCycle = 100;
  int enabled = 0;

  // call from setup to configure lightsleep
  void setup();

  // call from loop to check for lightsleep conditions
  void loop(int bleActive);
};

#endif