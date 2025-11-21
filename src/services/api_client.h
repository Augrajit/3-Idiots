#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <Arduino.h>
#include "../config/data_types.h"

bool api_init(String base_url);
void api_set_server(String ip, int port);
String api_call(String method, String endpoint, String payload, bool retry_on_timeout);
String api_face_verify(String rfid_uid, String face_base64);
bool api_log_transaction(Transaction t);
bool api_sync_offline_transactions(std::vector<Transaction> txns);
String api_get_balance(String student_id);
bool api_is_connected();

#endif

