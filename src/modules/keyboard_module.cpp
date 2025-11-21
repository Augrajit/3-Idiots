#include "keyboard_module.h"
#include <Arduino.h>
#include "../utils/logger.h"

// Row pins: GPIO 12, 14, 27, 26
// Column pins: GPIO 33, 32, 4, 2
const int ROW_PINS[4] = {12, 14, 27, 26};
const int COL_PINS[4] = {33, 32, 4, 2};

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
  for (int i = 0; i < 4; i++) {
    pinMode(ROW_PINS[i], OUTPUT);
    digitalWrite(ROW_PINS[i], HIGH);
  }
  
  // Set column pins as inputs with pull-up
  for (int i = 0; i < 4; i++) {
    pinMode(COL_PINS[i], INPUT_PULLUP);
  }
  
  Logger::logInfo("Keyboard: Initialized");
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

