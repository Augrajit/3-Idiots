#include "motion_sensor.h"
#include <Arduino.h>
#include "../utils/logger.h"

#define PIR_PIN 25
#define MOTION_DEBOUNCE_MS 2000 // 2 seconds

bool motion_state = false;
unsigned long last_motion_time = 0;
unsigned long last_high_time = 0;
bool consecutive_high = false;
void (*motion_callback)() = nullptr;

void motion_init() {
  pinMode(PIR_PIN, INPUT);
  motion_state = false;
  last_motion_time = 0;
  Logger::logInfo("Motion Sensor: Initialized");
}

bool motion_detected() {
  bool current_read = digitalRead(PIR_PIN) == HIGH;
  unsigned long now = millis();
  
  if (current_read) {
    if (!consecutive_high) {
      last_high_time = now;
      consecutive_high = true;
    }
    
    // Check if we've had HIGH for 2 seconds
    if (now - last_high_time >= MOTION_DEBOUNCE_MS) {
      if (!motion_state) {
        motion_state = true;
        last_motion_time = now;
        if (motion_callback != nullptr) {
          motion_callback();
        }
        Logger::logInfo("Motion: Detected");
      }
      return true;
    }
  } else {
    consecutive_high = false;
    // Motion stops after 1 second of LOW
    if (motion_state && (now - last_motion_time > 1000)) {
      motion_state = false;
      Logger::logInfo("Motion: Stopped");
    }
  }
  
  return motion_state;
}

unsigned long motion_last_time() {
  return last_motion_time;
}

void motion_set_callback(void (*callback)()) {
  motion_callback = callback;
}

