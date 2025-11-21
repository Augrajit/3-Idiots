#include "offline_service.h"
#include "../storage/transaction_cache.h"
#include "../services/api_client.h"
#include "../services/wifi_manager.h"
#include "../utils/logger.h"
#include <vector>

bool offline_mode_active = false;
std::vector<Transaction> offline_queue;

bool is_offline_mode() {
  offline_mode_active = !wifi_is_connected();
  return offline_mode_active;
}

FraudCheckResult check_offline_eligibility(String student_id) {
  FraudCheckResult result;
  result.passes_all_rules = false;
  result.requires_approval = true;
  result.alert_reason = "Offline mode - Limited verification";
  result.severity = 1;
  
  // Check local cache for double-serving (last 6 hours)
  std::vector<Transaction> recent_txns = cache_get_recent_transactions(6);
  int served_count = 0;
  for (const Transaction& txn : recent_txns) {
    if (txn.student_id == student_id && txn.status == "approved") {
      served_count++;
    }
  }
  
  if (served_count >= 1) {
    result.passes_all_rules = false;
    result.alert_reason = "Already served today (offline check)";
    result.severity = 2;
    return result;
  }
  
  // If no data available, require manager approval
  if (recent_txns.size() == 0) {
    result.requires_approval = true;
    result.alert_reason = "No local data - Manager approval required";
    return result;
  }
  
  // Basic check passed, but still require approval in offline mode
  result.passes_all_rules = true;
  result.requires_approval = true;
  result.alert_reason = "Offline mode - Manager approval required";
  
  return result;
}

bool transaction_can_proceed_offline(String student_id) {
  FraudCheckResult check = check_offline_eligibility(student_id);
  return check.passes_all_rules;
}

void queue_offline_transaction(Transaction t) {
  t.offline_mode = true;
  t.synced = false;
  offline_queue.push_back(t);
  Logger::logInfo("Offline: Queued transaction " + t.id);
}

void sync_offline_transactions() {
  if (!wifi_is_connected() || offline_queue.size() == 0) {
    return;
  }
  
  Logger::logInfo("Offline: Syncing " + String(offline_queue.size()) + " transactions");
  
  if (api_sync_offline_transactions(offline_queue)) {
    // Mark as synced in cache
    for (Transaction& t : offline_queue) {
      t.synced = true;
      cache_add_transaction(t); // Update cache with synced flag
    }
    
    offline_queue.clear();
    Logger::logInfo("Offline: All transactions synced");
  } else {
    Logger::logError("Offline: Sync failed, will retry later");
  }
}

std::vector<Transaction> get_offline_queue() {
  return offline_queue;
}

int get_offline_queue_size() {
  return offline_queue.size();
}

