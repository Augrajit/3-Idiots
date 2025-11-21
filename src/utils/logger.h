#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <stdarg.h>

class Logger {
public:
  static void init();
  static void log(String message);
  static void logf(const char* format, ...);
  static void logStateTransition(String from, String to);
  static void logError(String error);
  static void logInfo(String info);
};

#endif

