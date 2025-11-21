#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

#include <Arduino.h>

void motion_init();
bool motion_detected();
unsigned long motion_last_time();
void motion_set_callback(void (*callback)());

#endif

