#ifndef HELPERS_H
#define HELPERS_H

#include <Arduino.h>
#include "../config/data_types.h"

class Helpers {
public:
  static String getStateName(SystemState state);
  static unsigned long getCurrentTimestamp();
  static String base64Encode(uint8_t* data, size_t length);
};

#endif

