#include <Arduino.h>   // needed for PlatformIO
#include <Mesh.h>

#include "MyMesh.h"
#include <SerialReadLine.hpp>

#ifdef DISPLAY_CLASS
  #include "UITask.h"
  static UITask ui_task(display);
#endif

#ifdef ESP32
#include <helpers/esp32/LightSleep.h>
#endif

StdRNG fast_rng;
SimpleMeshTables tables;

SerialReadLine readline;

#ifdef ESP32
Esp32LightSleep lightsleep;
#endif

MyMesh the_mesh(board, radio_driver, *new ArduinoMillis(), fast_rng, rtc_clock, tables);

void halt() {
  while (1) ;
}

static char command[160];

void setup() {
  Serial.begin(115200);
  delay(1000);

  board.begin();

#ifdef DISPLAY_CLASS
  if (display.begin()) {
    display.startFrame();
    display.setCursor(0, 0);
    display.print("Please wait...");
    display.endFrame();
  }
#endif

  if (!radio_init()) {
    halt();
  }

  fast_rng.begin(radio_get_rng_seed());

  FILESYSTEM* fs;
#if defined(NRF52_PLATFORM) || defined(STM32_PLATFORM)
  InternalFS.begin();
  fs = &InternalFS;
  IdentityStore store(InternalFS, "");
#elif defined(ESP32)
  SPIFFS.begin(true);
  fs = &SPIFFS;
  IdentityStore store(SPIFFS, "/identity");
#elif defined(RP2040_PLATFORM)
  LittleFS.begin();
  fs = &LittleFS;
  IdentityStore store(LittleFS, "/identity");
  store.begin();
#else
  #error "need to define filesystem"
#endif
  if (!store.load("_main", the_mesh.self_id)) {
    MESH_DEBUG_PRINTLN("Generating new keypair");
    the_mesh.self_id = radio_new_identity();   // create new random identity
    int count = 0;
    while (count < 10 && (the_mesh.self_id.pub_key[0] == 0x00 || the_mesh.self_id.pub_key[0] == 0xFF)) {  // reserved id hashes
      the_mesh.self_id = radio_new_identity(); count++;
    }
    store.save("_main", the_mesh.self_id);
  }

  Serial.print("Repeater ID: ");
  mesh::Utils::printHex(Serial, the_mesh.self_id.pub_key, PUB_KEY_SIZE); Serial.println();

  command[0] = 0;

  sensors.begin();

  the_mesh.begin(fs);

#ifdef DISPLAY_CLASS
  ui_task.begin(the_mesh.getNodePrefs(), FIRMWARE_BUILD_DATE, FIRMWARE_VERSION);
#endif

  // send out initial Advertisement to the mesh
  the_mesh.sendSelfAdvertisement(16000);

  lightsleep.setup();
  lightsleep.enabled = true;
}

void loop() {
  bool bleActive = Serial.available();
  bool loraActive = (millis() - the_mesh.getLastPacketTime()) < 1000;
  int radioActive = bleActive + loraActive;

  if (auto command = readline.update()) {
    char reply[160];
    Serial.println();
    if (strcmp(command, "sleepy") == 0) {
      sprintf(reply, "sleepy en=%d duty=%d sleepcnt=%d", lightsleep.enabled, lightsleep.dutyCycle, lightsleep.sleepCnt);
    } else if (strcmp(command, "sleepy on") == 0) {
      lightsleep.enabled = true;
      strcpy(reply, "enabled");
    } else if (strcmp(command, "sleepy off") == 0) {
      lightsleep.enabled = false;
      strcpy(reply, "disabled");
    } else if (memcmp(command, "sleepy duty ", 12) == 0) {
      sscanf(command+12, "%d", &lightsleep.dutyCycle);
      sprintf(reply, "duty=%d of 100", lightsleep.dutyCycle);
    } else {
      the_mesh.handleCommand(0, command, reply);  // NOTE: there is no sender_timestamp via serial!
    }
    if (reply[0]) {
      Serial.print("  -> "); Serial.println(reply);
    }
  }

  the_mesh.loop();
  sensors.loop();
#ifdef DISPLAY_CLASS
  ui_task.loop();
#endif
  lightsleep.loop(radioActive);

  the_mesh.timeAwake = lightsleep.timeAwake.elapsed();
  the_mesh.timeAsleep = lightsleep.timeAsleep.elapsed();
}
