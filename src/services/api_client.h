#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <Arduino.h>
#include "../config/data_types.h"

void api_set_server(String ip, int port);
bool api_connect_wifi(String ssid, String password);
String api_call(String method, String endpoint, String payload);
FaceVerificationResult api_verify_face(String rfid_uid, String face_base64);
bool api_log_transaction(Transaction t);
bool api_sync_cache();
bool api_is_connected();

#endif

