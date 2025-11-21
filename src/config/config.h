#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "data_types.h"
#include "../utils/logger.h"

#define CONFIG_FILE "/config.json"

SystemConfig load_config() {
  SystemConfig config = SystemConfig::defaultConfig();
  
  if (!SPIFFS.begin(true)) {
    Logger::logError("Config: SPIFFS init failed");
    return config;
  }
  
  if (!SPIFFS.exists(CONFIG_FILE)) {
    Logger::logInfo("Config: File not found, using defaults");
    save_config(config);
    return config;
  }
  
  File file = SPIFFS.open(CONFIG_FILE, "r");
  if (!file) {
    Logger::logError("Config: Failed to open file");
    return config;
  }
  
  String content = file.readString();
  file.close();
  
  config = SystemConfig::fromJson(content);
  Logger::logInfo("Config: Loaded from file");
  return config;
}

bool save_config(SystemConfig config) {
  if (!SPIFFS.begin(true)) {
    return false;
  }
  
  File file = SPIFFS.open(CONFIG_FILE, "w");
  if (!file) {
    return false;
  }
  
  String json = config.toJson();
  file.print(json);
  file.close();
  
  Logger::logInfo("Config: Saved to file");
  return true;
}

#endif

