#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>

struct Transaction {
  String id; // Unique transaction ID
  unsigned long timestamp;
  String student_id;
  String student_name;
  String rfid_uid;
  String status; // "approved", "denied", "manual_approved", "override"
  float balance_before;
  float balance_after;
  String reason;
  bool fraud_alert;
  float face_confidence;
  bool synced; // Whether synced to server
  bool offline_mode; // Whether created in offline mode
  
  String toJson() {
    JsonDocument doc;
    doc["id"] = id;
    doc["timestamp"] = timestamp;
    doc["student_id"] = student_id;
    doc["student_name"] = student_name;
    doc["rfid_uid"] = rfid_uid;
    doc["status"] = status;
    doc["balance_before"] = balance_before;
    doc["balance_after"] = balance_after;
    doc["reason"] = reason;
    doc["fraud_alert"] = fraud_alert;
    doc["face_confidence"] = face_confidence;
    doc["synced"] = synced;
    doc["offline_mode"] = offline_mode;
    
    String result;
    serializeJson(doc, result);
    return result;
  }
  
  static Transaction fromJson(String json) {
    Transaction t;
    JsonDocument doc;
    deserializeJson(doc, json);
    
    t.id = doc["id"] | "";
    t.timestamp = doc["timestamp"] | 0;
    t.student_id = doc["student_id"] | "";
    t.student_name = doc["student_name"] | "";
    t.rfid_uid = doc["rfid_uid"] | "";
    t.status = doc["status"] | "";
    t.balance_before = doc["balance_before"] | 0.0;
    t.balance_after = doc["balance_after"] | 0.0;
    t.reason = doc["reason"] | "";
    t.fraud_alert = doc["fraud_alert"] | false;
    t.face_confidence = doc["face_confidence"] | 0.0;
    t.synced = doc["synced"] | false;
    t.offline_mode = doc["offline_mode"] | false;
    
    return t;
  }
};

struct FaceVerificationResult {
  bool success;
  String student_id;
  String student_name;
  float confidence;
  bool eligible;
  float balance;
  String meal_plan;
  bool already_served;
  bool needs_approval;
  String reason;
  
  static FaceVerificationResult fromJson(String json) {
    FaceVerificationResult result;
    JsonDocument doc;
    deserializeJson(doc, json);
    
    result.success = doc["status"] == "success";
    result.student_id = doc["student_id"] | "";
    result.student_name = doc["student_name"] | "";
    result.confidence = doc["confidence"] | 0.0;
    result.eligible = doc["eligible"] | false;
    result.balance = doc["balance"] | 0.0;
    result.meal_plan = doc["meal_plan"] | "";
    result.already_served = doc["already_served_today"] | false;
    result.needs_approval = doc["approval_required"] | false;
    result.reason = doc["reason"] | "";
    
    return result;
  }
};

struct FraudCheckResult {
  bool passes_all_rules;
  bool requires_approval;
  String alert_reason;
  int severity; // 0=none, 1=warning, 2=critical
  std::vector<String> triggered_rules;
};

enum ApprovalDecision {
  PENDING,
  APPROVED,
  DENIED,
  OVERRIDE
};

enum SystemState {
  IDLE,
  WAITING_FOR_CARD,
  CAPTURING_FACE,
  VERIFYING,
  DECISION,
  MANAGER_APPROVAL_WAIT,
  TRANSACTION_LOG,
  ERROR_STATE
};

struct SystemConfig {
  String server_ip;
  int server_port;
  String wifi_ssid;
  String wifi_password;
  int motion_timeout_sec;
  float min_face_confidence;
  bool offline_mode_enabled;
  
  static SystemConfig defaultConfig() {
    SystemConfig config;
    config.server_ip = "192.168.1.100";
    config.server_port = 5000;
    config.wifi_ssid = "";
    config.wifi_password = "";
    config.motion_timeout_sec = 30;
    config.min_face_confidence = 0.80;
    config.offline_mode_enabled = true;
    return config;
  }
  
  String toJson() {
    JsonDocument doc;
    doc["server_ip"] = server_ip;
    doc["server_port"] = server_port;
    doc["wifi_ssid"] = wifi_ssid;
    doc["wifi_password"] = wifi_password;
    doc["motion_timeout_sec"] = motion_timeout_sec;
    doc["min_face_confidence"] = min_face_confidence;
    doc["offline_mode_enabled"] = offline_mode_enabled;
    
    String result;
    serializeJson(doc, result);
    return result;
  }
  
  static SystemConfig fromJson(String json) {
    SystemConfig config = defaultConfig();
    JsonDocument doc;
    if (deserializeJson(doc, json) == DeserializationError::Ok) {
      config.server_ip = doc["server_ip"] | config.server_ip;
      config.server_port = doc["server_port"] | config.server_port;
      config.wifi_ssid = doc["wifi_ssid"] | config.wifi_ssid;
      config.wifi_password = doc["wifi_password"] | config.wifi_password;
      config.motion_timeout_sec = doc["motion_timeout_sec"] | config.motion_timeout_sec;
      config.min_face_confidence = doc["min_face_confidence"] | config.min_face_confidence;
      config.offline_mode_enabled = doc["offline_mode_enabled"] | config.offline_mode_enabled;
    }
    return config;
  }
};

#endif

