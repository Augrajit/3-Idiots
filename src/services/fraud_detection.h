#ifndef FRAUD_DETECTION_H
#define FRAUD_DETECTION_H

#include <Arduino.h>
#include <vector>
#include "../config/data_types.h"

FraudCheckResult check_fraud_rules(FaceVerificationResult fvr, 
                                    std::vector<Transaction> recent_txns);

#endif

