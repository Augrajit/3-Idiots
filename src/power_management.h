#ifndef POWER_MANAGEMENT_H
#define POWER_MANAGEMENT_H

#include <Arduino.h>

void power_init();
void enter_sleep_mode();
void exit_sleep_mode();
unsigned long get_uptime();
bool is_sleep_mode();
void power_check_sleep(unsigned long motion_timeout_ms);

#endif

