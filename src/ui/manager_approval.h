#ifndef MANAGER_APPROVAL_H
#define MANAGER_APPROVAL_H

#include <Arduino.h>
#include "../config/data_types.h"
#include "../modules/keyboard_module.h"

ApprovalDecision wait_manager_approval(String student_name, 
                                       String reason,
                                       int timeout_sec);

#endif

