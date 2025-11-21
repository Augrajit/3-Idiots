#ifndef FRAUD_DETECTION_H
#define FRAUD_DETECTION_H

#include <Arduino.h>
#include <vector>
#include "../config/data_types.h"

FraudCheckResult check_all_fraud_rules(String rfid_uid,
                                       FaceVerificationResult fvr, 
                                       std::vector<Transaction> recent_txns);

#endif

