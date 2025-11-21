#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Arduino.h>

bool oled_init();
void display_status(String status, String balance, bool approved);
void display_waiting(String msg);
void display_error(String error_msg);
void display_manager_approval_screen(String student_name, String id, String reason);
void clear_display();

#endif

