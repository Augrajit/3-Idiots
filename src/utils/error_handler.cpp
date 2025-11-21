#include "error_handler.h"
#include "../modules/oled_display.h"
#include "../utils/logger.h"

ErrorCode last_error = ERR_NO_ERROR;
String last_error_context = "";

void handle_error(ErrorCode code, String context) {
  last_error = code;
  last_error_context = context;
  
  Logger::logError("Error [" + error_to_string(code) + "]: " + context);
  
  String message = get_user_message(code);
  String recovery = get_error_recovery_action(code);
  
  display_error(message + "\n" + recovery);
}

String error_to_string(ErrorCode code) {
  switch(code) {
    case ERR_NO_ERROR: return "NO_ERROR";
    case ERR_RFID_TIMEOUT: return "RFID_TIMEOUT";
    case ERR_FACE_CAPTURE_FAIL: return "FACE_CAPTURE_FAIL";
    case ERR_FACE_RECOGNITION_FAIL: return "FACE_RECOGNITION_FAIL";
    case ERR_API_TIMEOUT: return "API_TIMEOUT";
    case ERR_API_INVALID_RESPONSE: return "API_INVALID_RESPONSE";
    case ERR_DOUBLE_SERVING: return "DOUBLE_SERVING";
    case ERR_INSUFFICIENT_BALANCE: return "INSUFFICIENT_BALANCE";
    case ERR_EXPIRED_CARD: return "EXPIRED_CARD";
    case ERR_NETWORK_OFFLINE: return "NETWORK_OFFLINE";
    case ERR_HARDWARE_FAULT: return "HARDWARE_FAULT";
    case ERR_MANAGER_DENIED: return "MANAGER_DENIED";
    default: return "UNKNOWN_ERROR";
  }
}

String get_user_message(ErrorCode code) {
  switch(code) {
    case ERR_NO_ERROR: return "System OK";
    case ERR_RFID_TIMEOUT: return "Card read timeout";
    case ERR_FACE_CAPTURE_FAIL: return "Face capture failed";
    case ERR_FACE_RECOGNITION_FAIL: return "Face recognition failed";
    case ERR_API_TIMEOUT: return "Network timeout";
    case ERR_API_INVALID_RESPONSE: return "Server error";
    case ERR_DOUBLE_SERVING: return "Already served today";
    case ERR_INSUFFICIENT_BALANCE: return "Insufficient balance";
    case ERR_EXPIRED_CARD: return "Card expired";
    case ERR_NETWORK_OFFLINE: return "Network offline";
    case ERR_HARDWARE_FAULT: return "Hardware fault";
    case ERR_MANAGER_DENIED: return "Manager denied";
    default: return "Unknown error";
  }
}

String get_error_recovery_action(ErrorCode code) {
  switch(code) {
    case ERR_RFID_TIMEOUT: return "Retry card scan";
    case ERR_FACE_CAPTURE_FAIL: return "Retry face capture";
    case ERR_FACE_RECOGNITION_FAIL: return "Try again or contact manager";
    case ERR_API_TIMEOUT: return "Using offline mode";
    case ERR_API_INVALID_RESPONSE: return "Contact administrator";
    case ERR_DOUBLE_SERVING: return "Contact manager";
    case ERR_INSUFFICIENT_BALANCE: return "Add balance";
    case ERR_EXPIRED_CARD: return "Renew card";
    case ERR_NETWORK_OFFLINE: return "Offline mode active";
    case ERR_HARDWARE_FAULT: return "Contact support";
    case ERR_MANAGER_DENIED: return "Contact manager";
    default: return "Try again";
  }
}

bool can_recover_from_error(ErrorCode code) {
  switch(code) {
    case ERR_RFID_TIMEOUT:
    case ERR_FACE_CAPTURE_FAIL:
    case ERR_API_TIMEOUT:
    case ERR_NETWORK_OFFLINE:
      return true;
    default:
      return false;
  }
}

ErrorCode get_last_error() {
  return last_error;
}

String get_last_error_context() {
  return last_error_context;
}

