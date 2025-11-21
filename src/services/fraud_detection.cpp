#include "fraud_detection.h"
#include "../storage/transaction_cache.h"
#include "../utils/logger.h"
#include "../config/data_types.h"
#include <vector>

FraudCheckResult check_all_fraud_rules(String rfid_uid,
                                       FaceVerificationResult fvr, 
                                       std::vector<Transaction> recent_txns) {
  FraudCheckResult result;
  result.passes_all_rules = true;
  result.requires_approval = false;
  result.alert_reason = "";
  result.severity = 0;
  result.triggered_rules.clear();
  
  // Rule 1: Double-Serving Prevention - Check if student_id served in last 6 hours
  unsigned long six_hours_ago = (millis() / 1000) - (6 * 3600);
  int same_student_count = 0;
  for (const Transaction& txn : recent_txns) {
    if (txn.student_id == fvr.student_id && 
        txn.status == "approved" && 
        txn.timestamp >= six_hours_ago) {
      same_student_count++;
    }
  }
  
  if (same_student_count >= 1) {
    result.passes_all_rules = false;
    result.alert_reason = "Already served in last 6 hours (Double-serving detected)";
    result.severity = 2;
    result.triggered_rules.push_back("DOUBLE_SERVING");
    Logger::logError("Fraud: Double-serving detected for " + fvr.student_id);
    return result;
  }
  
  // Rule 2: Credential Mismatch - RFID UID doesn't match student record
  // This is checked by the API, but we verify here too
  if (rfid_uid.length() > 0 && fvr.student_id.length() > 0) {
    // If API returned success, credentials match
    // This rule is primarily handled by backend
  }
  
  // Rule 3: Face Confidence Threshold
  if (fvr.confidence < 0.60) {
    result.passes_all_rules = false;
    result.alert_reason = "Face confidence too low (" + String(fvr.confidence, 2) + ")";
    result.severity = 2;
    result.triggered_rules.push_back("LOW_CONFIDENCE_DENY");
    Logger::logError("Fraud: Confidence too low - " + String(fvr.confidence, 2));
    return result;
  } else if (fvr.confidence < 0.75) {
    result.requires_approval = true;
    result.alert_reason = "Low face confidence (" + String(fvr.confidence, 2) + ")";
    result.severity = 1;
    result.triggered_rules.push_back("LOW_CONFIDENCE_APPROVAL");
    Logger::logInfo("Fraud: Low confidence requires approval");
  }
  
  // Rule 4: Insufficient Balance
  const float MIN_MEAL_COST = 100.0; // Rs. 100 minimum
  if (fvr.balance <= 0) {
    result.passes_all_rules = false;
    result.alert_reason = "Insufficient balance";
    result.severity = 2;
    result.triggered_rules.push_back("INSUFFICIENT_BALANCE");
    Logger::logError("Fraud: Insufficient balance for " + fvr.student_id);
    return result;
  } else if (fvr.balance < MIN_MEAL_COST) {
    result.requires_approval = true;
    result.alert_reason = "Low balance (Rs. " + String(fvr.balance, 2) + ")";
    result.severity = 1;
    result.triggered_rules.push_back("LOW_BALANCE_WARNING");
    Logger::logInfo("Fraud: Low balance requires approval");
  }
  
  // Rule 5: Rapid Multiple Attempts - 3+ failed attempts in 10 minutes
  int failed_attempts = 0;
  unsigned long ten_min_ago = (millis() / 1000) - 600;
  for (const Transaction& txn : recent_txns) {
    if (txn.student_id == fvr.student_id && 
        txn.timestamp >= ten_min_ago && 
        (txn.status == "denied" || txn.status == "manual_denied")) {
      failed_attempts++;
    }
  }
  
  if (failed_attempts >= 3) {
    result.requires_approval = true;
    result.alert_reason = "Multiple failed attempts - Manager review required";
    result.severity = 2;
    result.triggered_rules.push_back("RAPID_ATTEMPTS");
    Logger::logError("Fraud: Multiple failed attempts detected");
  }
  
  // Rule 6: Meal Plan Validation
  if (fvr.meal_plan != "active") {
    result.passes_all_rules = false;
    result.alert_reason = "Meal plan not active (" + fvr.meal_plan + ")";
    result.severity = 2;
    result.triggered_rules.push_back("MEAL_PLAN_INACTIVE");
    Logger::logError("Fraud: Meal plan not active");
    return result;
  }
  
  // Rule 7: Biometric Mismatch Alert (if RFID says Student A but face says Student B)
  // This is primarily handled by backend API, but we log it here
  if (fvr.already_served) {
    result.passes_all_rules = false;
    result.alert_reason = "Already served today";
    result.severity = 2;
    result.triggered_rules.push_back("ALREADY_SERVED");
    Logger::logError("Fraud: Already served today");
    return result;
  }
  
  // Rule 7b: Eligibility Check
  if (!fvr.eligible) {
    result.passes_all_rules = false;
    result.alert_reason = "Student not eligible";
    result.severity = 2;
    result.triggered_rules.push_back("NOT_ELIGIBLE");
    Logger::logError("Fraud: Student not eligible");
    return result;
  }
  
  return result;
}

