#include "power_management.h"
#include <esp_pm.h>
#include <esp32-hal-cpu.h>
#include "../modules/motion_sensor.h"
#include "../modules/oled_display.h"
#include "../utils/logger.h"

unsigned long last_motion_time = 0;
bool sleep_mode = false;
uint32_t original_cpu_freq = 240;

void power_init() {
  // Set initial CPU frequency
  setCpuFrequencyMhz(240);
  Logger::logInfo("Power Management: Initialized");
}

void enter_sleep_mode() {
  if (sleep_mode) {
    return;
  }
  
  Logger::logInfo("Power Management: Entering sleep mode");
  
  // Turn off OLED (clear display)
  clear_display();
  
  // Reduce CPU frequency to 80MHz
  original_cpu_freq = getCpuFrequencyMhz();
  setCpuFrequencyMhz(80);
  
  // Note: PIR sensor stays active for wake interrupt
  sleep_mode = true;
}

void exit_sleep_mode() {
  if (!sleep_mode) {
    return;
  }
  
  Logger::logInfo("Power Management: Exiting sleep mode");
  
  // Restore CPU frequency
  setCpuFrequencyMhz(original_cpu_freq);
  
  // OLED will be turned on by display functions
  sleep_mode = false;
}

unsigned long get_uptime() {
  return millis() / 1000;
}

bool is_sleep_mode() {
  return sleep_mode;
}

void power_check_sleep(unsigned long motion_timeout_ms) {
  unsigned long now = millis();
  unsigned long time_since_motion = now - motion_last_time();
  
  if (time_since_motion > motion_timeout_ms && !sleep_mode) {
    enter_sleep_mode();
  } else if (time_since_motion <= motion_timeout_ms && sleep_mode) {
    exit_sleep_mode();
  }
}

