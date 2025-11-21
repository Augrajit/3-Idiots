#include "logger.h"
#include <Arduino.h>

#define DEBUG 1

#if DEBUG
  #define LOG(msg) Serial.println(msg)
  #define LOGF(fmt, ...) Serial.printf(fmt, __VA_ARGS__)
#else
  #define LOG(msg)
  #define LOGF(fmt, ...)
#endif

void Logger::init() {
  Serial.begin(115200);
  delay(1000);
  LOG("Logger initialized");
}

void Logger::log(String message) {
  LOG(message);
}

void Logger::logf(const char* format, ...) {
  va_list args;
  va_start(args, format);
  char buffer[256];
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
  LOG(buffer);
}

void Logger::logStateTransition(String from, String to) {
  LOGF("[STATE] %s -> %s", from.c_str(), to.c_str());
}

void Logger::logError(String error) {
  LOGF("[ERROR] %s", error.c_str());
}

void Logger::logInfo(String info) {
  LOGF("[INFO] %s", info.c_str());
}

