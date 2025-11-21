#include "api_client.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "../utils/logger.h"
#include "../config/data_types.h"

String server_base_url = "";
int server_port = 5000;
bool wifi_connected = false;

void api_set_server(String ip, int port) {
  server_base_url = ip;
  server_port = port;
}

bool api_connect_wifi(String ssid, String password) {
  if (WiFi.status() == WL_CONNECTED) {
    wifi_connected = true;
    return true;
  }
  
  Logger::logInfo("WiFi: Connecting to " + ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifi_connected = true;
    Logger::logInfo("WiFi: Connected - " + WiFi.localIP().toString());
    return true;
  } else {
    wifi_connected = false;
    Logger::logError("WiFi: Connection failed");
    return false;
  }
}

String api_call(String method, String endpoint, String payload) {
  if (!wifi_connected || WiFi.status() != WL_CONNECTED) {
    Logger::logError("API: WiFi not connected");
    return "";
  }
  
  HTTPClient http;
  String url = "http://" + server_base_url + ":" + String(server_port) + endpoint;
  
  http.begin(url);
  http.setTimeout(3000); // 3 second timeout
  http.addHeader("Content-Type", "application/json");
  
  int httpCode = 0;
  if (method == "POST") {
    httpCode = http.POST(payload);
  } else if (method == "GET") {
    httpCode = http.GET();
  }
  
  String response = "";
  if (httpCode > 0) {
    response = http.getString();
    Logger::logInfo("API: " + method + " " + endpoint + " - Code: " + String(httpCode));
  } else {
    Logger::logError("API: Request failed - " + String(httpCode));
  }
  
  http.end();
  return response;
}

FaceVerificationResult api_verify_face(String rfid_uid, String face_base64) {
  FaceVerificationResult result;
  result.success = false;
  
  JsonDocument doc;
  doc["rfid_uid"] = rfid_uid;
  doc["face_image"] = face_base64;
  doc["timestamp"] = millis() / 1000;
  
  String payload;
  serializeJson(doc, payload);
  
  String response = api_call("POST", "/api/face/verify", payload);
  
  if (response.length() > 0) {
    result = FaceVerificationResult::fromJson(response);
  } else {
    result.reason = "Network timeout or error";
  }
  
  return result;
}

bool api_log_transaction(Transaction t) {
  String payload = t.toJson();
  String response = api_call("POST", "/api/transactions/log", payload);
  return response.length() > 0;
}

bool api_sync_cache() {
  // This would sync all cached transactions
  // For now, just return true if WiFi is connected
  return wifi_connected;
}

bool api_is_connected() {
  return wifi_connected && WiFi.status() == WL_CONNECTED;
}

