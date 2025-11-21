#include "dining_system.h"
#include "../modules/rfid_module.h"
#include "../modules/oled_display.h"
#include "../modules/keyboard_module.h"
#include "../modules/motion_sensor.h"
#include "../modules/esp_cam_module.h"
#include "../services/api_client.h"
#include "../services/fraud_detection.h"
#include "../services/face_recognition_service.h"
#include "../storage/transaction_cache.h"
#include "../ui/manager_approval.h"
#include "../power_management.h"
#include "../utils/logger.h"
#include "../utils/helpers.h"
#include <vector>

DiningSystem::DiningSystem() {
  current_state = IDLE;
  current_rfid_uid = "";
  current_face_image = "";
  last_state_change = millis();
}

void DiningSystem::init(SystemConfig config) {
  system_config = config;
  
  // Initialize all modules
  Logger::init();
  Logger::logInfo("=== IOTrix Dining System Starting ===");
  
  if (!oled_init()) {
    Logger::logError("Failed to initialize OLED");
  }
  
  if (!rfid_init()) {
    Logger::logError("Failed to initialize RFID");
  }
  
  keyboard_init();
  motion_init();
  
  if (!esp_cam_init()) {
    Logger::logError("Failed to initialize ESP-CAM");
  }
  
  if (!cache_init()) {
    Logger::logError("Failed to initialize transaction cache");
  }
  
  power_init();
  
  // Connect WiFi
  if (config.wifi_ssid.length() > 0) {
    api_connect_wifi(config.wifi_ssid, config.wifi_password);
    api_set_server(config.server_ip, config.server_port);
  }
  
  // Callbacks will be handled in update() method
  
  display_status("System Ready", "0.00", false);
  Logger::logInfo("System initialized successfully");
}

void DiningSystem::update() {
  // Check motion sensor
  bool motion = motion_detected();
  
  // Handle motion callback manually
  if (motion && current_state == IDLE && !is_sleep_mode()) {
    transition_to(WAITING_FOR_CARD);
  }
  
  // Check power management
  power_check_sleep(system_config.motion_timeout_sec * 1000);
  
  // Update state machine
  switch (current_state) {
    case IDLE:
      state_idle();
      break;
    case WAITING_FOR_CARD:
      state_waiting_for_card();
      break;
    case CAPTURING_FACE:
      state_capturing_face();
      break;
    case VERIFYING:
      state_verifying();
      break;
    case DECISION:
      state_decision();
      break;
    case MANAGER_APPROVAL_WAIT:
      state_manager_approval_wait();
      break;
    case TRANSACTION_LOG:
      state_transaction_log();
      break;
  }
  
  // Scan keyboard
  keyboard_scan();
  
  delay(20); // 50Hz loop
}

void DiningSystem::state_idle() {
  // Motion handling is done in update()
}

void DiningSystem::state_waiting_for_card() {
  display_waiting("Scan Card");
  
  if (rfid_detect_card()) {
    current_rfid_uid = rfid_read_uid();
    if (current_rfid_uid.length() > 0) {
      Logger::logInfo("RFID: Card detected - " + current_rfid_uid);
      transition_to(CAPTURING_FACE);
    }
  }
  
  // Timeout after 30 seconds
  if (millis() - last_state_change > 30000) {
    transition_to(IDLE);
  }
}

void DiningSystem::state_capturing_face() {
  display_waiting("Capturing Face");
  
  if (esp_cam_capture_frame()) {
    current_face_image = esp_cam_get_base64_jpeg();
    if (current_face_image.length() > 0) {
      Logger::logInfo("Camera: Face captured");
      esp_cam_cleanup();
      transition_to(VERIFYING);
    } else {
      Logger::logError("Camera: Failed to encode image");
      display_error("Face capture failed");
      delay(2000);
      transition_to(WAITING_FOR_CARD);
    }
  } else {
    Logger::logError("Camera: Capture failed");
    display_error("Camera error");
    delay(2000);
    transition_to(WAITING_FOR_CARD);
  }
}

void DiningSystem::state_verifying() {
  display_waiting("Verifying");
  
  // Call API to verify face + RFID
  FaceVerificationResult fvr;
  
  if (api_is_connected()) {
    fvr = api_verify_face(current_rfid_uid, current_face_image);
  } else {
    // Offline mode - basic check
    Logger::logInfo("Offline mode: Limited verification");
    fvr.success = false;
    fvr.reason = "Network unavailable - Offline mode";
    fvr.needs_approval = true;
  }
  
  if (fvr.success) {
    current_verification_result = fvr;
    Logger::logInfo("Verification: Success - " + fvr.student_name);
    
    // Check fraud rules
    std::vector<Transaction> recent_txns = cache_get_recent_transactions(6);
    current_fraud_result = check_fraud_rules(fvr, recent_txns);
    
    transition_to(DECISION);
  } else {
    Logger::logError("Verification: Failed - " + fvr.reason);
    display_error("Verification failed: " + fvr.reason);
    delay(3000);
    transition_to(WAITING_FOR_CARD);
  }
}

void DiningSystem::state_decision() {
  // Check if fraud rules pass
  if (!current_fraud_result.passes_all_rules) {
    // Deny transaction
    Logger::logError("Decision: DENIED - " + current_fraud_result.alert_reason);
    create_transaction("denied", current_fraud_result.alert_reason);
    display_error(current_fraud_result.alert_reason);
    delay(3000);
    transition_to(TRANSACTION_LOG);
    return;
  }
  
  // Check if approval required
  if (current_fraud_result.requires_approval || current_verification_result.needs_approval) {
    transition_to(MANAGER_APPROVAL_WAIT);
    return;
  }
  
  // Auto-approve
  Logger::logInfo("Decision: AUTO-APPROVED");
  create_transaction("approved", "Auto-approved - matched credentials");
  display_status("APPROVED", String(current_verification_result.balance, 2), true);
  delay(2000);
  transition_to(TRANSACTION_LOG);
}

void DiningSystem::state_manager_approval_wait() {
  String reason = current_fraud_result.alert_reason.length() > 0 ? 
                  current_fraud_result.alert_reason : 
                  current_verification_result.reason;
  
  ApprovalDecision decision = wait_manager_approval(
    current_verification_result.student_name,
    reason,
    60
  );
  
  if (decision == APPROVED) {
    Logger::logInfo("Manager: APPROVED");
    create_transaction("manual_approved", "Manager approved");
    display_status("APPROVED", String(current_verification_result.balance, 2), true);
    delay(2000);
  } else if (decision == OVERRIDE) {
    Logger::logInfo("Manager: OVERRIDE");
    create_transaction("override", "Manager override");
    display_status("OVERRIDE", String(current_verification_result.balance, 2), true);
    delay(2000);
  } else {
    Logger::logInfo("Manager: DENIED");
    create_transaction("denied", "Manager denied");
    display_error("Contact Manager");
    delay(3000);
  }
  
  transition_to(TRANSACTION_LOG);
}

void DiningSystem::state_transaction_log() {
  // Log transaction to cache and API
  if (current_transaction.student_id.length() > 0) {
    cache_add_transaction(current_transaction);
    
    if (api_is_connected()) {
      api_log_transaction(current_transaction);
    }
  }
  
  // Wait a bit then return to idle
  delay(2000);
  transition_to(IDLE);
}

void DiningSystem::transition_to(SystemState next_state) {
  String from = Helpers::getStateName(current_state);
  String to = Helpers::getStateName(next_state);
  Logger::logStateTransition(from, to);
  
  current_state = next_state;
  last_state_change = millis();
  
  // Clear state variables on transition
  if (next_state == WAITING_FOR_CARD) {
    current_rfid_uid = "";
    current_face_image = "";
  }
}

void DiningSystem::handle_keyboard_input(int key) {
  // Handle keyboard input during manager approval
  if (current_state == MANAGER_APPROVAL_WAIT) {
    // Keyboard handling is done in wait_manager_approval
    return;
  }
}

void DiningSystem::create_transaction(String status, String reason) {
  Transaction t;
  t.timestamp = Helpers::getCurrentTimestamp();
  t.student_id = current_verification_result.student_id;
  t.student_name = current_verification_result.student_name;
  t.rfid_uid = current_rfid_uid;
  t.status = status;
  t.balance_before = current_verification_result.balance;
  t.balance_after = current_verification_result.balance - 5.0; // Assuming meal cost
  t.reason = reason;
  t.fraud_alert = current_fraud_result.severity >= 2;
  t.face_confidence = current_verification_result.confidence;
  
  current_transaction = t;
}

SystemState DiningSystem::get_state() {
  return current_state;
}

