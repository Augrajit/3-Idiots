#ifndef MANAGER_APPROVAL_H
#define MANAGER_APPROVAL_H

#include <Arduino.h>
#include "../config/data_types.h"
#include "../modules/keyboard_module.h"

ApprovalDecision wait_manager_approval(String student_name, 
                                       String student_id,
                                       String reason,
                                       int timeout_sec);
void log_manager_action(ApprovalDecision decision, String student_id, String reason);

#endif

