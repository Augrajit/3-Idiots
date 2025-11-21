#include <Arduino.h>
#include "app/dining_system.h"
#include "config/data_types.h"
#include "config/config.h"
#include "utils/logger.h"

DiningSystem diningSystem;

void setup() {
  // Load config from SPIFFS or use defaults
  SystemConfig config = load_config();
  
  // Override with hardcoded values if needed (for initial setup)
  // These can be set via web portal later
  if (config.wifi_ssid.length() == 0) {
    config.wifi_ssid = "BDSET";
    config.wifi_password = "Bdset@1234";
    config.server_ip = "192.168.1.100";  // Change this
    config.server_port = 5000;
    save_config(config);  // Save for next boot
  }
  
  diningSystem.init(config);
}

void loop() {
  diningSystem.update();
}

