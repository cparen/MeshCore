/* ------------------------------ Config -------------------------------- */
#ifndef RADIOCONFIG_H
#define RADIOCONFIG_H

#ifndef LORA_FREQ
  #define LORA_FREQ   910.525
#endif
#ifndef LORA_BW
  #define LORA_BW     62.5
#endif
#ifndef LORA_SF
  #define LORA_SF     7
#endif
#ifndef LORA_CR
  #define LORA_CR      5
#endif
#ifndef LORA_TX_POWER
  #define LORA_TX_POWER  20
#endif

// Naming convention:
// c/cc 7XXX <ccc7..NN>
#ifndef ADVERT_NAME
  #define  ADVERT_NAME   "c/cc fox test"
#endif
#ifndef ADVERT_LAT
  #define  ADVERT_LAT  0.0
#endif
#ifndef ADVERT_LON
  #define  ADVERT_LON  0.0
#endif

#ifndef ADMIN_PASSWORD
  #define  ADMIN_PASSWORD  "snowbunny"
#endif

#ifndef SERVER_RESPONSE_DELAY
  #define SERVER_RESPONSE_DELAY   300
#endif

#ifndef TXT_ACK_DELAY
  #define TXT_ACK_DELAY     200
#endif

#ifndef SENSOR_READ_INTERVAL_SECS
  #define SENSOR_READ_INTERVAL_SECS  60
#endif

#endif
