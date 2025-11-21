#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>

bool wifi_init(String ssid, String password);
bool wifi_is_connected();
int wifi_get_signal_strength();
void wifi_check_connection();
String wifi_get_status_string();
void wifi_set_status_callback(void (*callback)(bool connected));
bool wifi_load_credentials();

#endif

