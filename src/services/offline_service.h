#ifndef OFFLINE_SERVICE_H
#define OFFLINE_SERVICE_H

#include <Arduino.h>
#include <vector>
#include "../config/data_types.h"

bool is_offline_mode();
FraudCheckResult check_offline_eligibility(String student_id);
bool transaction_can_proceed_offline(String student_id);
void queue_offline_transaction(Transaction t);
void sync_offline_transactions();
std::vector<Transaction> get_offline_queue();
int get_offline_queue_size();

#endif

