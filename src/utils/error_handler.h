#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <Arduino.h>

enum ErrorCode {
  ERR_NO_ERROR = 0,
  ERR_RFID_TIMEOUT = 1,
  ERR_FACE_CAPTURE_FAIL = 2,
  ERR_FACE_RECOGNITION_FAIL = 3,
  ERR_API_TIMEOUT = 4,
  ERR_API_INVALID_RESPONSE = 5,
  ERR_DOUBLE_SERVING = 6,
  ERR_INSUFFICIENT_BALANCE = 7,
  ERR_EXPIRED_CARD = 8,
  ERR_NETWORK_OFFLINE = 9,
  ERR_HARDWARE_FAULT = 10,
  ERR_MANAGER_DENIED = 11
};

void handle_error(ErrorCode code, String context);
String error_to_string(ErrorCode code);
String get_user_message(ErrorCode code);
String get_error_recovery_action(ErrorCode code);
bool can_recover_from_error(ErrorCode code);
ErrorCode get_last_error();
String get_last_error_context();

#endif

