#include "api_client.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "../utils/logger.h"
#include "../config/data_types.h"
#include "../services/wifi_manager.h"

String server_base_url = "";
int server_port = 5000;
const int API_TIMEOUT = 5000;
const int MAX_RETRIES = 1;

void api_set_server(String ip, int port) {
  server_base_url = ip;
  server_port = port;
}

bool api_init(String base_url) {
  server_base_url = base_url;
  Logger::logInfo("API Client: Initialized with base URL " + base_url);
  return true;
}

String api_call(String method, String endpoint, String payload, bool retry_on_timeout) {
  if (!wifi_is_connected()) {
    Logger::logError("API: WiFi not connected");
    return "";
  }
  
  int retries = retry_on_timeout ? MAX_RETRIES : 0;
  
  for (int attempt = 0; attempt <= retries; attempt++) {
    HTTPClient http;
    String url = "http://" + server_base_url + ":" + String(server_port) + endpoint;
    
    http.begin(url);
    http.setTimeout(API_TIMEOUT);
    http.addHeader("Content-Type", "application/json");
    
    int httpCode = 0;
    if (method == "POST") {
      httpCode = http.POST(payload);
    } else if (method == "GET") {
      httpCode = http.GET();
    }
    
    String response = "";
    if (httpCode > 0 && httpCode < 500) {
      response = http.getString();
      Logger::logInfo("API: " + method + " " + endpoint + " - Code: " + String(httpCode));
      http.end();
      return response;
    } else {
      Logger::logError("API: Request failed - Code: " + String(httpCode));
      if (attempt < retries) {
        Logger::logInfo("API: Retrying...");
        delay(500);
      }
    }
    
    http.end();
  }
  
  return "";
}

String api_face_verify(String rfid_uid, String face_base64) {
  JsonDocument doc;
  doc["rfid_uid"] = rfid_uid;
  doc["face_image"] = face_base64;
  doc["timestamp"] = millis() / 1000;
  
  String payload;
  serializeJson(doc, payload);
  
  return api_call("POST", "/api/auth/face-verify", payload, true);
}

bool api_log_transaction(Transaction t) {
  JsonDocument doc;
  doc["student_id"] = t.student_id;
  doc["rfid_uid"] = t.rfid_uid;
  doc["face_confidence"] = t.face_confidence;
  doc["approval_status"] = t.status;
  doc["approval_type"] = (t.status == "approved") ? "auto" : 
                         (t.status == "manual_approved") ? "manual" : 
                         (t.status == "override") ? "override" : "denied";
  doc["balance_before"] = t.balance_before;
  doc["balance_after"] = t.balance_after;
  doc["timestamp"] = t.timestamp;
  doc["fraud_detected"] = t.fraud_alert;
  doc["reason"] = t.reason;
  
  String payload;
  serializeJson(doc, payload);
  
  String response = api_call("POST", "/api/transactions/log", payload, false);
  return response.length() > 0;
}

bool api_sync_offline_transactions(std::vector<Transaction> txns) {
  JsonDocument doc;
  doc["device_id"] = "esp32_device_001";
  JsonArray arr = doc["transactions"].to<JsonArray>();
  
  for (const Transaction& t : txns) {
    JsonObject obj = arr.add<JsonObject>();
    obj["id"] = t.id;
    obj["timestamp"] = t.timestamp;
    obj["student_id"] = t.student_id;
    obj["student_name"] = t.student_name;
    obj["rfid_uid"] = t.rfid_uid;
    obj["status"] = t.status;
    obj["balance_before"] = t.balance_before;
    obj["balance_after"] = t.balance_after;
    obj["reason"] = t.reason;
    obj["fraud_alert"] = t.fraud_alert;
    obj["face_confidence"] = t.face_confidence;
  }
  
  String payload;
  serializeJson(doc, payload);
  
  String response = api_call("POST", "/api/transactions/sync-batch", payload, true);
  
  if (response.length() > 0) {
    JsonDocument respDoc;
    if (deserializeJson(respDoc, response) == DeserializationError::Ok) {
      int synced = respDoc["synced_count"] | 0;
      Logger::logInfo("API: Synced " + String(synced) + " transactions");
      return synced > 0;
    }
  }
  
  return false;
}

String api_get_balance(String student_id) {
  String endpoint = "/api/student/" + student_id + "/balance";
  return api_call("GET", endpoint, "", false);
}

bool api_is_connected() {
  return wifi_is_connected();
}

