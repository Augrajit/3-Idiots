#include "fraud_detection.h"
#include "../storage/transaction_cache.h"
#include "../utils/logger.h"
#include "../config/data_types.h"

FraudCheckResult check_fraud_rules(FaceVerificationResult fvr, 
                                    std::vector<Transaction> recent_txns) {
  FraudCheckResult result;
  result.passes_all_rules = true;
  result.requires_approval = false;
  result.alert_reason = "";
  result.severity = 0;
  
  // Rule 1: Double-Serving Rule - Same student_id appears twice in last 6 hours
  int same_student_count = 0;
  for (const Transaction& txn : recent_txns) {
    if (txn.student_id == fvr.student_id && txn.status == "approved") {
      same_student_count++;
    }
  }
  
  if (same_student_count >= 1) {
    result.passes_all_rules = false;
    result.alert_reason = "Already served today (Double-serving detected)";
    result.severity = 2;
    Logger::logError("Fraud: Double-serving detected for " + fvr.student_id);
    return result;
  }
  
  // Rule 2: Balance Rule - Balance <= 0
  if (fvr.balance <= 0) {
    result.passes_all_rules = false;
    result.alert_reason = "Insufficient balance";
    result.severity = 2;
    Logger::logError("Fraud: Insufficient balance for " + fvr.student_id);
    return result;
  }
  
  // Rule 3: Face Confidence Rule - Confidence < 0.80
  if (fvr.confidence < 0.80) {
    result.requires_approval = true;
    result.alert_reason = "Low face confidence (" + String(fvr.confidence, 2) + ")";
    result.severity = 1;
    Logger::logInfo("Fraud: Low confidence requires approval");
  }
  
  // Rule 4: Already Served Today
  if (fvr.already_served) {
    result.passes_all_rules = false;
    result.alert_reason = "Already served today";
    result.severity = 2;
    Logger::logError("Fraud: Already served today");
    return result;
  }
  
  // Rule 5: Meal Plan Check
  if (fvr.meal_plan != "active") {
    result.passes_all_rules = false;
    result.alert_reason = "Meal plan not active";
    result.severity = 2;
    Logger::logError("Fraud: Meal plan not active");
    return result;
  }
  
  // Rule 6: Rapid Multiple Attempts - 3+ failed attempts in 10 min
  int failed_attempts = 0;
  unsigned long ten_min_ago = (millis() / 1000) - 600;
  for (const Transaction& txn : recent_txns) {
    if (txn.student_id == fvr.student_id && 
        txn.timestamp >= ten_min_ago && 
        txn.status == "denied") {
      failed_attempts++;
    }
  }
  
  if (failed_attempts >= 3) {
    result.requires_approval = true;
    result.alert_reason = "Multiple failed attempts - Manager review required";
    result.severity = 2;
    Logger::logError("Fraud: Multiple failed attempts detected");
  }
  
  // Rule 7: Eligibility Check
  if (!fvr.eligible) {
    result.passes_all_rules = false;
    result.alert_reason = "Student not eligible";
    result.severity = 2;
    Logger::logError("Fraud: Student not eligible");
    return result;
  }
  
  return result;
}

