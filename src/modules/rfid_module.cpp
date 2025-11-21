#include "rfid_module.h"
#include <SPI.h>
#include <MFRC522.h>
#include "../utils/logger.h"

// ESP32-CAM AI-Thinker Pinout:
// Left side: GPIO 12, 13, 15, 14, 2, 4
// Right side: GPIO 16, 0
// Available GPIO: 0, 2, 4, 12, 13, 14, 15, 16 (8 pins total)
// RFID uses SPI (default pins: 23-MOSI, 19-MISO, 18-SCK are internal camera pins)
#define RFID_SS_PIN 2    // GPIO 2 (HS2_DATA0) - Left side pin 7
#define RFID_RST_PIN 4   // GPIO 4 (HS2_DATA1/FLASH) - Left side pin 8

MFRC522 mfrc522(RFID_SS_PIN, RFID_RST_PIN);
bool rfid_initialized = false;

bool rfid_init() {
  if (rfid_initialized) {
    return true;
  }
  
  SPI.begin();
  mfrc522.PCD_Init();
  
  // Set SPI frequency to 10MHz
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
  
  // Test communication
  byte version = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  if (version == 0x00 || version == 0xFF) {
    Logger::logError("RFID: Communication failed");
    return false;
  }
  
  rfid_initialized = true;
  Logger::logInfo("RFID: Initialized successfully");
  return true;
}

bool rfid_detect_card() {
  if (!rfid_initialized) {
    return false;
  }
  
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return false;
  }
  
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return false;
  }
  
  return true;
}

String rfid_read_uid() {
  if (!rfid_initialized) {
    return "";
  }
  
  if (!rfid_detect_card()) {
    return "";
  }
  
  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) {
      uid += "0";
    }
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  
  // Stop reading
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  
  return uid;
}

void rfid_stop() {
  if (rfid_initialized) {
    mfrc522.PCD_Init();
    rfid_initialized = false;
    Logger::logInfo("RFID: Stopped");
  }
}

