#ifndef TRANSACTION_CACHE_H
#define TRANSACTION_CACHE_H

#include <Arduino.h>
#include <vector>
#include "../config/data_types.h"

bool cache_init();
bool cache_add_transaction(Transaction t);
std::vector<Transaction> cache_get_today_transactions(String student_id);
std::vector<Transaction> cache_get_all_today();
bool cache_student_served_today(String student_id);
std::vector<Transaction> cache_get_unsynced();
bool cache_mark_synced(String transaction_id);
std::vector<Transaction> cache_get_recent_transactions(int hours);
void cache_clear_old_entries(int days);

#endif

