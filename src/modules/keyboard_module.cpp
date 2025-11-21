#include "keyboard_module.h"
#include <Arduino.h>
#include "../utils/logger.h"

// ESP32-CAM AI-Thinker Pinout (8 GPIO pins available):
// Left side: GPIO 12, 13, 15, 14, 2, 4
// Right side: GPIO 16, 0
// Row pins: GPIO 14, 15, 16, 0
//   - GPIO 14 (HS2_CLK) - Left side pin 6
//   - GPIO 15 (HS2_CMD) - Left side pin 5
//   - GPIO 16 (U2RXD) - Right side pin 2, shared with PIR
//   - GPIO 0 (CSI_MCLK) - Right side pin 3, camera clock (can be used as GPIO)
// Column pins: GPIO 13, 12, 4, 2 (shared with other components)
//   - GPIO 13: Shared with OLED SCL (I2C) - scan when I2C idle
//   - GPIO 12: Shared with OLED SDA (I2C) - scan when I2C idle
//   - GPIO 4: Shared with RFID RST - scan when RFID idle
//   - GPIO 2: Shared with RFID SS - scan when RFID idle
const int ROW_PINS[4] = {14, 15, 16, 0};
const int COL_PINS[4] = {13, 12, 4, 2};

// Key mapping
// [1] [2] [3] [APPROVE]
// [4] [5] [6] [DENY]
// [7] [8] [9] [RETRY]
// [*] [0] [#] [OVERRIDE]
const int KEY_MAP[4][4] = {
  {1, 2, 3, KEY_APPROVE},
  {4, 5, 6, KEY_DENY},
  {7, 8, 9, KEY_RETRY},
  {KEY_STAR, 0, KEY_HASH, KEY_OVERRIDE}
};

void (*keyboard_callback)(int key) = nullptr;
int last_key = -1;
unsigned long last_key_time = 0;
const unsigned long DEBOUNCE_TIME = 20; // 20ms debounce

void keyboard_init() {
  // Set row pins as outputs (HIGH)
  // Note: GPIO 0 is boot pin - keeping it HIGH prevents boot issues
  for (int i = 0; i < 4; i++) {
    pinMode(ROW_PINS[i], OUTPUT);
    digitalWrite(ROW_PINS[i], HIGH);
  }
  
  // Set column pins as inputs with pull-up
  // Note: These pins are shared with I2C (12,13) and RFID (2,4)
  // Keyboard scanning should be done when those peripherals are idle
  for (int i = 0; i < 4; i++) {
    pinMode(COL_PINS[i], INPUT_PULLUP);
  }
  
  Logger::logInfo("Keyboard: Initialized (pins shared with I2C/RFID)");
}

int keyboard_get_key() {
  unsigned long now = millis();
  
  // Scan matrix
  for (int row = 0; row < 4; row++) {
    // Set current row LOW
    digitalWrite(ROW_PINS[row], LOW);
    delayMicroseconds(10); // Small delay for settling
    
    // Check each column
    for (int col = 0; col < 4; col++) {
      if (digitalRead(COL_PINS[col]) == LOW) {
        int key = KEY_MAP[row][col];
        
        // Debounce check
        if (key == last_key && (now - last_key_time) < DEBOUNCE_TIME) {
          digitalWrite(ROW_PINS[row], HIGH);
          return -1; // Still debouncing
        }
        
        // New key press
        if (key != last_key || (now - last_key_time) >= DEBOUNCE_TIME) {
          last_key = key;
          last_key_time = now;
          digitalWrite(ROW_PINS[row], HIGH);
          return key;
        }
      }
    }
    
    // Set row back HIGH
    digitalWrite(ROW_PINS[row], HIGH);
  }
  
  // No key pressed
  if (now - last_key_time > 100) {
    last_key = -1;
  }
  
  return -1;
}

void keyboard_set_callback(void (*callback)(int key)) {
  keyboard_callback = callback;
}

void keyboard_scan() {
  int key = keyboard_get_key();
  if (key != -1 && keyboard_callback != nullptr) {
    keyboard_callback(key);
  }
}

