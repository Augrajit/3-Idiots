#include "helpers.h"
#include <Arduino.h>

String Helpers::getStateName(SystemState state) {
  switch(state) {
    case IDLE: return "IDLE";
    case WAITING_FOR_CARD: return "WAITING_FOR_CARD";
    case CAPTURING_FACE: return "CAPTURING_FACE";
    case VERIFYING: return "VERIFYING";
    case DECISION: return "DECISION";
    case MANAGER_APPROVAL_WAIT: return "MANAGER_APPROVAL_WAIT";
    case TRANSACTION_LOG: return "TRANSACTION_LOG";
    default: return "UNKNOWN";
  }
}

unsigned long Helpers::getCurrentTimestamp() {
  return millis() / 1000; // Simplified - should use NTP in production
}

String Helpers::base64Encode(uint8_t* data, size_t length) {
  const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  String encoded = "";
  int i = 0;
  int j = 0;
  uint8_t char_array_3[3];
  uint8_t char_array_4[4];
  
  while (length--) {
    char_array_3[i++] = *(data++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;
      
      for(i = 0; i < 4; i++) {
        encoded += base64_chars[char_array_4[i]];
      }
      i = 0;
    }
  }
  
  if (i) {
    for(j = i; j < 3; j++) {
      char_array_3[j] = '\0';
    }
    
    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;
    
    for (j = 0; j < i + 1; j++) {
      encoded += base64_chars[char_array_4[j]];
    }
    
    while((i++ < 3)) {
      encoded += '=';
    }
  }
  
  return encoded;
}

