#ifndef LIGHTSLEEP_H
#define LIGHTSLEEP_H

namespace esp32 {

  // call from setup to configure lightsleep
  void lightsleep_setup();

  // call from loop to check for lightsleep conditions
  void lightsleep_loop(int bleActive);
}

#endif