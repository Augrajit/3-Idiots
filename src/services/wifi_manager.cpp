#include "wifi_manager.h"
#include <WiFi.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "../utils/logger.h"

#define WIFI_CREDENTIALS_FILE "/wifi_creds.json"
#define MAX_RETRY_DELAY 30000
#define INITIAL_RETRY_DELAY 2000
#define CONNECTION_TIMEOUT 20000

bool wifi_initialized = false;
bool wifi_connected = false;
String wifi_ssid = "";
String wifi_password = "";
int retry_delay = INITIAL_RETRY_DELAY;
unsigned long last_connection_attempt = 0;
unsigned long last_health_check = 0;
void (*status_callback)(bool connected) = nullptr;

bool wifi_init(String ssid, String password) {
  wifi_ssid = ssid;
  wifi_password = password;
  
  // Save credentials to SPIFFS
  if (SPIFFS.begin(true)) {
    JsonDocument doc;
    doc["ssid"] = ssid;
    doc["password"] = password;
    
    File file = SPIFFS.open(WIFI_CREDENTIALS_FILE, "w");
    if (file) {
      serializeJson(doc, file);
      file.close();
    }
  }
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  wifi_initialized = true;
  retry_delay = INITIAL_RETRY_DELAY;
  
  Logger::logInfo("WiFi Manager: Initialized");
  return wifi_connect();
}

bool wifi_connect() {
  if (wifi_ssid.length() == 0) {
    Logger::logError("WiFi: No SSID configured");
    return false;
  }
  
  Logger::logInfo("WiFi: Connecting to " + wifi_ssid);
  WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
  
  unsigned long start_time = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - start_time) < CONNECTION_TIMEOUT) {
    delay(500);
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifi_connected = true;
    retry_delay = INITIAL_RETRY_DELAY; // Reset on success
    Logger::logInfo("WiFi: Connected - IP: " + WiFi.localIP().toString());
    if (status_callback) status_callback(true);
    return true;
  } else {
    wifi_connected = false;
    Logger::logError("WiFi: Connection failed");
    if (status_callback) status_callback(false);
    return false;
  }
}

bool wifi_is_connected() {
  if (WiFi.status() == WL_CONNECTED) {
    if (!wifi_connected) {
      wifi_connected = true;
      retry_delay = INITIAL_RETRY_DELAY;
      Logger::logInfo("WiFi: Reconnected");
      if (status_callback) status_callback(true);
    }
    return true;
  } else {
    if (wifi_connected) {
      wifi_connected = false;
      Logger::logError("WiFi: Connection lost");
      if (status_callback) status_callback(false);
    }
    return false;
  }
}

int wifi_get_signal_strength() {
  if (!wifi_is_connected()) {
    return 0;
  }
  return WiFi.RSSI();
}

void wifi_check_connection() {
  if (!wifi_initialized) {
    return;
  }
  
  unsigned long now = millis();
  
  // Health check every 30 seconds
  if (now - last_health_check > 30000) {
    last_health_check = now;
    if (!wifi_is_connected()) {
      // Try to reconnect
      wifi_auto_reconnect();
    }
  }
}

void wifi_auto_reconnect() {
  if (wifi_is_connected()) {
    return;
  }
  
  unsigned long now = millis();
  
  // Exponential backoff: 2s, 4s, 8s, 16s, 30s max
  if (now - last_connection_attempt < retry_delay) {
    return; // Still in backoff period
  }
  
  last_connection_attempt = now;
  
  Logger::logInfo("WiFi: Attempting reconnect (delay: " + String(retry_delay) + "ms)");
  
  if (wifi_connect()) {
    retry_delay = INITIAL_RETRY_DELAY;
  } else {
    // Exponential backoff
    retry_delay = min(retry_delay * 2, MAX_RETRY_DELAY);
  }
}

String wifi_get_status_string() {
  if (!wifi_initialized) {
    return "Not Initialized";
  }
  
  if (wifi_is_connected()) {
    int rssi = wifi_get_signal_strength();
    return "Connected (" + String(rssi) + " dBm)";
  } else {
    return "Disconnected";
  }
}

void wifi_set_status_callback(void (*callback)(bool connected)) {
  status_callback = callback;
}

bool wifi_load_credentials() {
  if (!SPIFFS.begin(true)) {
    return false;
  }
  
  if (!SPIFFS.exists(WIFI_CREDENTIALS_FILE)) {
    return false;
  }
  
  File file = SPIFFS.open(WIFI_CREDENTIALS_FILE, "r");
  if (!file) {
    return false;
  }
  
  String content = file.readString();
  file.close();
  
  JsonDocument doc;
  if (deserializeJson(doc, content) == DeserializationError::Ok) {
    wifi_ssid = doc["ssid"] | "";
    wifi_password = doc["password"] | "";
    return wifi_ssid.length() > 0;
  }
  
  return false;
}

