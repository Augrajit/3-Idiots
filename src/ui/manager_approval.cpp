#include "manager_approval.h"
#include "../modules/oled_display.h"
#include "../modules/keyboard_module.h"
#include "../utils/logger.h"
#include "../config/data_types.h"

ApprovalDecision wait_manager_approval(String student_name, 
                                       String student_id,
                                       String reason,
                                       int timeout_sec) {
  Logger::logInfo("Manager Approval: Waiting for decision");
  
  display_manager_approval_screen(student_name, student_id, reason);
  
  unsigned long start_time = millis();
  unsigned long timeout_ms = timeout_sec * 1000;
  unsigned long last_display_update = 0;
  
  while (millis() - start_time < timeout_ms) {
    int key = keyboard_get_key();
    
    if (key == KEY_APPROVE) {
      Logger::logInfo("Manager Approval: APPROVED");
      log_manager_action(APPROVED, student_id, reason);
      return APPROVED;
    } else if (key == KEY_DENY) {
      Logger::logInfo("Manager Approval: DENIED");
      log_manager_action(DENIED, student_id, reason);
      return DENIED;
    } else if (key == KEY_OVERRIDE) {
      Logger::logInfo("Manager Approval: OVERRIDE (forced)");
      log_manager_action(OVERRIDE, student_id, reason);
      return OVERRIDE;
    }
    
    // Update display every 500ms to show timeout countdown
    if (millis() - last_display_update > 500) {
      unsigned long remaining = (timeout_ms - (millis() - start_time)) / 1000;
      display_manager_approval_screen(student_name, student_id, reason + " (" + String(remaining) + "s)");
      last_display_update = millis();
    }
    
    delay(50);
  }
  
  Logger::logInfo("Manager Approval: Timeout - Auto DENY");
  log_manager_action(DENIED, student_id, "Timeout - Auto denied");
  return DENIED;
}

void log_manager_action(ApprovalDecision decision, String student_id, String reason) {
  String decision_str = "";
  switch(decision) {
    case APPROVED: decision_str = "APPROVED"; break;
    case DENIED: decision_str = "DENIED"; break;
    case OVERRIDE: decision_str = "OVERRIDE"; break;
    default: decision_str = "PENDING"; break;
  }
  
  Logger::logInfo("Manager Action: " + decision_str + " for " + student_id + " - " + reason);
  // TODO: Store in SPIFFS for audit trail
}

