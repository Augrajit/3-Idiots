#include "oled_display.h"
#include <U8g2lib.h>
#include <Wire.h>
#include "../utils/logger.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

bool oled_initialized = false;

bool oled_init() {
  if (oled_initialized) {
    return true;
  }
  
  Wire.begin(21, 22); // SDA, SCL
  u8g2.begin();
  u8g2.setFontMode(1);
  u8g2.setFontDirection(0);
  
  oled_initialized = true;
  Logger::logInfo("OLED: Initialized successfully");
  return true;
}

void display_status(String status, String balance, bool approved) {
  if (!oled_initialized) return;
  
  u8g2.clearBuffer();
  
  // Header
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 10, "IOTrix Dining System");
  
  // Main message
  u8g2.setFont(u8g2_font_ncenB14_tr);
  int statusWidth = u8g2.getStrWidth(status.c_str());
  u8g2.drawStr((128 - statusWidth) / 2, 35, status.c_str());
  
  // Status indicator
  u8g2.setFont(u8g2_font_ncenB10_tr);
  if (approved) {
    u8g2.drawStr(0, 50, "APPROVED");
  } else {
    u8g2.drawStr(0, 50, "PENDING");
  }
  
  // Balance
  String balanceStr = "Balance: $" + balance;
  int balanceWidth = u8g2.getStrWidth(balanceStr.c_str());
  u8g2.drawStr(128 - balanceWidth, 63, balanceStr.c_str());
  
  u8g2.sendBuffer();
}

void display_waiting(String msg) {
  if (!oled_initialized) return;
  
  static int dotCount = 0;
  static unsigned long lastUpdate = 0;
  unsigned long now = millis();
  
  if (now - lastUpdate > 500) {
    dotCount = (dotCount + 1) % 4;
    lastUpdate = now;
  }
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB10_tr);
  
  String displayMsg = msg;
  for (int i = 0; i < dotCount; i++) {
    displayMsg += ".";
  }
  
  int msgWidth = u8g2.getStrWidth(displayMsg.c_str());
  u8g2.drawStr((128 - msgWidth) / 2, 32, displayMsg.c_str());
  
  u8g2.sendBuffer();
}

void display_error(String error_msg) {
  if (!oled_initialized) return;
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 10, "ERROR");
  
  u8g2.setFont(u8g2_font_ncenR08_tr);
  // Word wrap for long messages
  int y = 25;
  String remaining = error_msg;
  while (remaining.length() > 0 && y < 64) {
    String line = "";
    int chars = 0;
    while (chars < remaining.length() && u8g2.getStrWidth((line + remaining[chars]).c_str()) < 120) {
      line += remaining[chars];
      chars++;
    }
    u8g2.drawStr(0, y, line.c_str());
    remaining = remaining.substring(chars);
    y += 10;
  }
  
  u8g2.sendBuffer();
}

void display_manager_approval_screen(String student_name, String id, String reason) {
  if (!oled_initialized) return;
  
  u8g2.clearBuffer();
  
  // Header
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 10, "MANAGER APPROVAL");
  
  // Name
  u8g2.setFont(u8g2_font_ncenB10_tr);
  String nameLabel = "Name: " + student_name;
  u8g2.drawStr(0, 25, nameLabel.c_str());
  
  // ID
  u8g2.setFont(u8g2_font_ncenR08_tr);
  String idLabel = "ID: " + id;
  u8g2.drawStr(0, 38, idLabel.c_str());
  
  // Reason
  u8g2.setFont(u8g2_font_ncenR08_tr);
  String reasonLabel = "Reason: " + reason;
  u8g2.drawStr(0, 50, reasonLabel.c_str());
  
  // Buttons hint
  u8g2.setFont(u8g2_font_ncenR08_tr);
  u8g2.drawStr(0, 63, "[APPROVE] [DENY]");
  
  u8g2.sendBuffer();
}

void clear_display() {
  if (!oled_initialized) return;
  u8g2.clearBuffer();
  u8g2.sendBuffer();
}

