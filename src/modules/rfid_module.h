#ifndef RFID_MODULE_H
#define RFID_MODULE_H

#include <Arduino.h>

bool rfid_init();
bool rfid_detect_card();
String rfid_read_uid();
void rfid_stop();

#endif

