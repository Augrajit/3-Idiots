#include "manager_approval.h"
#include "../modules/oled_display.h"
#include "../modules/keyboard_module.h"
#include "../utils/logger.h"
#include "../config/data_types.h"

ApprovalDecision wait_manager_approval(String student_name, 
                                       String reason,
                                       int timeout_sec) {
  Logger::logInfo("Manager Approval: Waiting for decision");
  
  display_manager_approval_screen(student_name, "", reason);
  
  unsigned long start_time = millis();
  unsigned long timeout_ms = timeout_sec * 1000;
  
  while (millis() - start_time < timeout_ms) {
    int key = keyboard_get_key();
    
    if (key == KEY_APPROVE) {
      Logger::logInfo("Manager Approval: APPROVED");
      return APPROVED;
    } else if (key == KEY_DENY) {
      Logger::logInfo("Manager Approval: DENIED");
      return DENIED;
    } else if (key == KEY_OVERRIDE) {
      Logger::logInfo("Manager Approval: OVERRIDE (forced)");
      return OVERRIDE;
    }
    
    delay(50);
  }
  
  Logger::logInfo("Manager Approval: Timeout - Auto DENY");
  return DENIED;
}

