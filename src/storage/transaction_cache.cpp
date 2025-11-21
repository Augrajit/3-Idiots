#include "transaction_cache.h"
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <vector>
#include "../utils/logger.h"

#define CACHE_FILE "/transactions.json"
#define MAX_CACHED_TRANSACTIONS 100

bool cache_initialized = false;

bool cache_init() {
  if (cache_initialized) {
    return true;
  }
  
  if (!SPIFFS.begin(true)) {
    Logger::logError("Cache: SPIFFS init failed");
    return false;
  }
  
  cache_initialized = true;
  Logger::logInfo("Transaction Cache: Initialized");
  return true;
}

bool cache_add_transaction(Transaction t) {
  if (!cache_initialized) {
    return false;
  }
  
  // Generate ID if not present
  if (t.id.length() == 0) {
    t.id = "TXN_" + String(millis()) + "_" + String(random(1000, 9999));
  }
  
  // Read existing transactions
  std::vector<Transaction> transactions;
  if (SPIFFS.exists(CACHE_FILE)) {
    File file = SPIFFS.open(CACHE_FILE, "r");
    if (file) {
      String content = file.readString();
      file.close();
      
      JsonDocument doc;
      if (deserializeJson(doc, content) == DeserializationError::Ok) {
        JsonArray arr = doc["transactions"].as<JsonArray>();
        for (JsonObject obj : arr) {
          Transaction tx;
          tx.id = obj["id"] | "";
          tx.timestamp = obj["timestamp"] | 0;
          tx.student_id = obj["student_id"] | "";
          tx.student_name = obj["student_name"] | "";
          tx.rfid_uid = obj["rfid_uid"] | "";
          tx.status = obj["status"] | "";
          tx.balance_before = obj["balance_before"] | 0.0;
          tx.balance_after = obj["balance_after"] | 0.0;
          tx.reason = obj["reason"] | "";
          tx.fraud_alert = obj["fraud_alert"] | false;
          tx.face_confidence = obj["face_confidence"] | 0.0;
          tx.synced = obj["synced"] | false;
          tx.offline_mode = obj["offline_mode"] | false;
          transactions.push_back(tx);
        }
      }
    }
  }
  
  // Add new transaction
  transactions.push_back(t);
  
  // Keep only last MAX_CACHED_TRANSACTIONS
  if (transactions.size() > MAX_CACHED_TRANSACTIONS) {
    transactions.erase(transactions.begin(), 
                      transactions.begin() + (transactions.size() - MAX_CACHED_TRANSACTIONS));
  }
  
  // Write back to file
  JsonDocument doc;
  JsonArray arr = doc["transactions"].to<JsonArray>();
  for (const Transaction& tx : transactions) {
    JsonObject obj = arr.add<JsonObject>();
    obj["id"] = tx.id;
    obj["timestamp"] = tx.timestamp;
    obj["student_id"] = tx.student_id;
    obj["student_name"] = tx.student_name;
    obj["rfid_uid"] = tx.rfid_uid;
    obj["status"] = tx.status;
    obj["balance_before"] = tx.balance_before;
    obj["balance_after"] = tx.balance_after;
    obj["reason"] = tx.reason;
    obj["fraud_alert"] = tx.fraud_alert;
    obj["face_confidence"] = tx.face_confidence;
    obj["synced"] = tx.synced;
    obj["offline_mode"] = tx.offline_mode;
  }
  
  File file = SPIFFS.open(CACHE_FILE, "w");
  if (!file) {
    Logger::logError("Cache: Failed to write file");
    return false;
  }
  
  serializeJson(doc, file);
  file.close();
  
  return true;
}

std::vector<Transaction> cache_get_today_transactions(String student_id) {
  std::vector<Transaction> result;
  
  if (!cache_initialized || !SPIFFS.exists(CACHE_FILE)) {
    return result;
  }
  
  File file = SPIFFS.open(CACHE_FILE, "r");
  if (!file) {
    return result;
  }
  
  String content = file.readString();
  file.close();
  
  unsigned long today_start = (millis() / 1000) - (24 * 3600); // Last 24 hours
  
  JsonDocument doc;
  if (deserializeJson(doc, content) == DeserializationError::Ok) {
    JsonArray arr = doc["transactions"].as<JsonArray>();
    for (JsonObject obj : arr) {
      unsigned long ts = obj["timestamp"] | 0;
      String sid = obj["student_id"] | "";
      
      if (sid == student_id && ts >= today_start) {
        Transaction tx;
        tx.timestamp = ts;
        tx.student_id = sid;
        tx.student_name = obj["student_name"] | "";
        tx.rfid_uid = obj["rfid_uid"] | "";
        tx.status = obj["status"] | "";
        tx.balance_before = obj["balance_before"] | 0.0;
        tx.balance_after = obj["balance_after"] | 0.0;
        tx.reason = obj["reason"] | "";
        tx.fraud_alert = obj["fraud_alert"] | false;
        tx.face_confidence = obj["face_confidence"] | 0.0;
        result.push_back(tx);
      }
    }
  }
  
  return result;
}

std::vector<Transaction> cache_get_recent_transactions(int hours) {
  std::vector<Transaction> result;
  
  if (!cache_initialized || !SPIFFS.exists(CACHE_FILE)) {
    return result;
  }
  
  File file = SPIFFS.open(CACHE_FILE, "r");
  if (!file) {
    return result;
  }
  
  String content = file.readString();
  file.close();
  
  unsigned long time_threshold = (millis() / 1000) - (hours * 3600);
  
  JsonDocument doc;
  if (deserializeJson(doc, content) == DeserializationError::Ok) {
    JsonArray arr = doc["transactions"].as<JsonArray>();
    for (JsonObject obj : arr) {
      unsigned long ts = obj["timestamp"] | 0;
      
      if (ts >= time_threshold) {
        Transaction tx;
        tx.id = obj["id"] | "";
        tx.timestamp = ts;
        tx.student_id = obj["student_id"] | "";
        tx.student_name = obj["student_name"] | "";
        tx.rfid_uid = obj["rfid_uid"] | "";
        tx.status = obj["status"] | "";
        tx.balance_before = obj["balance_before"] | 0.0;
        tx.balance_after = obj["balance_after"] | 0.0;
        tx.reason = obj["reason"] | "";
        tx.fraud_alert = obj["fraud_alert"] | false;
        tx.face_confidence = obj["face_confidence"] | 0.0;
        tx.synced = obj["synced"] | false;
        tx.offline_mode = obj["offline_mode"] | false;
        result.push_back(tx);
      }
    }
  }
  
  return result;
}

void cache_clear_old_entries(int days) {
  if (!cache_initialized || !SPIFFS.exists(CACHE_FILE)) {
    return;
  }
  
  File file = SPIFFS.open(CACHE_FILE, "r");
  if (!file) {
    return;
  }
  
  String content = file.readString();
  file.close();
  
  unsigned long time_threshold = (millis() / 1000) - (days * 24 * 3600);
  std::vector<Transaction> transactions;
  
  JsonDocument doc;
  if (deserializeJson(doc, content) == DeserializationError::Ok) {
    JsonArray arr = doc["transactions"].as<JsonArray>();
    for (JsonObject obj : arr) {
      unsigned long ts = obj["timestamp"] | 0;
      
      if (ts >= time_threshold) {
        Transaction tx;
        tx.timestamp = ts;
        tx.student_id = obj["student_id"] | "";
        tx.student_name = obj["student_name"] | "";
        tx.rfid_uid = obj["rfid_uid"] | "";
        tx.status = obj["status"] | "";
        tx.balance_before = obj["balance_before"] | 0.0;
        tx.balance_after = obj["balance_after"] | 0.0;
        tx.reason = obj["reason"] | "";
        tx.fraud_alert = obj["fraud_alert"] | false;
        tx.face_confidence = obj["face_confidence"] | 0.0;
        transactions.push_back(tx);
      }
    }
  }
  
  // Write back filtered transactions
  JsonDocument newDoc;
  JsonArray newArr = newDoc["transactions"].to<JsonArray>();
  for (const Transaction& tx : transactions) {
    JsonObject obj = newArr.add<JsonObject>();
    obj["timestamp"] = tx.timestamp;
    obj["student_id"] = tx.student_id;
    obj["student_name"] = tx.student_name;
    obj["rfid_uid"] = tx.rfid_uid;
    obj["status"] = tx.status;
    obj["balance_before"] = tx.balance_before;
    obj["balance_after"] = tx.balance_after;
    obj["reason"] = tx.reason;
    obj["fraud_alert"] = tx.fraud_alert;
    obj["face_confidence"] = tx.face_confidence;
  }
  
  file = SPIFFS.open(CACHE_FILE, "w");
  if (file) {
    serializeJson(newDoc, file);
    file.close();
  }
}

std::vector<Transaction> cache_get_all_today() {
  std::vector<Transaction> result;
  unsigned long today_start = (millis() / 1000) - (24 * 3600);
  
  if (!cache_initialized || !SPIFFS.exists(CACHE_FILE)) {
    return result;
  }
  
  File file = SPIFFS.open(CACHE_FILE, "r");
  if (!file) {
    return result;
  }
  
  String content = file.readString();
  file.close();
  
  JsonDocument doc;
  if (deserializeJson(doc, content) == DeserializationError::Ok) {
    JsonArray arr = doc["transactions"].as<JsonArray>();
    for (JsonObject obj : arr) {
      unsigned long ts = obj["timestamp"] | 0;
      if (ts >= today_start) {
        Transaction tx;
        tx.id = obj["id"] | "";
        tx.timestamp = ts;
        tx.student_id = obj["student_id"] | "";
        tx.student_name = obj["student_name"] | "";
        tx.rfid_uid = obj["rfid_uid"] | "";
        tx.status = obj["status"] | "";
        tx.balance_before = obj["balance_before"] | 0.0;
        tx.balance_after = obj["balance_after"] | 0.0;
        tx.reason = obj["reason"] | "";
        tx.fraud_alert = obj["fraud_alert"] | false;
        tx.face_confidence = obj["face_confidence"] | 0.0;
        tx.synced = obj["synced"] | false;
        tx.offline_mode = obj["offline_mode"] | false;
        result.push_back(tx);
      }
    }
  }
  
  return result;
}

bool cache_student_served_today(String student_id) {
  std::vector<Transaction> today_txns = cache_get_today_transactions(student_id);
  for (const Transaction& txn : today_txns) {
    if (txn.status == "approved" || txn.status == "manual_approved" || txn.status == "override") {
      return true;
    }
  }
  return false;
}

std::vector<Transaction> cache_get_unsynced() {
  std::vector<Transaction> result;
  
  if (!cache_initialized || !SPIFFS.exists(CACHE_FILE)) {
    return result;
  }
  
  File file = SPIFFS.open(CACHE_FILE, "r");
  if (!file) {
    return result;
  }
  
  String content = file.readString();
  file.close();
  
  JsonDocument doc;
  if (deserializeJson(doc, content) == DeserializationError::Ok) {
    JsonArray arr = doc["transactions"].as<JsonArray>();
    for (JsonObject obj : arr) {
      bool synced = obj["synced"] | false;
      if (!synced) {
        Transaction tx;
        tx.id = obj["id"] | "";
        tx.timestamp = obj["timestamp"] | 0;
        tx.student_id = obj["student_id"] | "";
        tx.student_name = obj["student_name"] | "";
        tx.rfid_uid = obj["rfid_uid"] | "";
        tx.status = obj["status"] | "";
        tx.balance_before = obj["balance_before"] | 0.0;
        tx.balance_after = obj["balance_after"] | 0.0;
        tx.reason = obj["reason"] | "";
        tx.fraud_alert = obj["fraud_alert"] | false;
        tx.face_confidence = obj["face_confidence"] | 0.0;
        tx.synced = false;
        tx.offline_mode = obj["offline_mode"] | false;
        result.push_back(tx);
      }
    }
  }
  
  return result;
}

bool cache_mark_synced(String transaction_id) {
  if (!cache_initialized || !SPIFFS.exists(CACHE_FILE)) {
    return false;
  }
  
  File file = SPIFFS.open(CACHE_FILE, "r");
  if (!file) {
    return false;
  }
  
  String content = file.readString();
  file.close();
  
  std::vector<Transaction> transactions;
  JsonDocument doc;
  if (deserializeJson(doc, content) == DeserializationError::Ok) {
    JsonArray arr = doc["transactions"].as<JsonArray>();
    for (JsonObject obj : arr) {
      Transaction tx;
      tx.id = obj["id"] | "";
      tx.timestamp = obj["timestamp"] | 0;
      tx.student_id = obj["student_id"] | "";
      tx.student_name = obj["student_name"] | "";
      tx.rfid_uid = obj["rfid_uid"] | "";
      tx.status = obj["status"] | "";
      tx.balance_before = obj["balance_before"] | 0.0;
      tx.balance_after = obj["balance_after"] | 0.0;
      tx.reason = obj["reason"] | "";
      tx.fraud_alert = obj["fraud_alert"] | false;
      tx.face_confidence = obj["face_confidence"] | 0.0;
      tx.synced = (tx.id == transaction_id) ? true : (obj["synced"] | false);
      tx.offline_mode = obj["offline_mode"] | false;
      transactions.push_back(tx);
    }
  }
  
  // Write back
  JsonDocument newDoc;
  JsonArray newArr = newDoc["transactions"].to<JsonArray>();
  for (const Transaction& tx : transactions) {
    JsonObject obj = newArr.add<JsonObject>();
    obj["id"] = tx.id;
    obj["timestamp"] = tx.timestamp;
    obj["student_id"] = tx.student_id;
    obj["student_name"] = tx.student_name;
    obj["rfid_uid"] = tx.rfid_uid;
    obj["status"] = tx.status;
    obj["balance_before"] = tx.balance_before;
    obj["balance_after"] = tx.balance_after;
    obj["reason"] = tx.reason;
    obj["fraud_alert"] = tx.fraud_alert;
    obj["face_confidence"] = tx.face_confidence;
    obj["synced"] = tx.synced;
    obj["offline_mode"] = tx.offline_mode;
  }
  
  file = SPIFFS.open(CACHE_FILE, "w");
  if (file) {
    serializeJson(newDoc, file);
    file.close();
    return true;
  }
  
  return false;
}

