#!/usr/bin/env bash
esptool -p /dev/ttyUSB0 --chip esp32-s3 write-flash 0x10000 .pio/build/heltec_v3/firmware.bin