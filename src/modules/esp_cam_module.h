#ifndef ESP_CAM_MODULE_H
#define ESP_CAM_MODULE_H

#include <Arduino.h>

bool esp_cam_init();
bool esp_cam_capture_frame();
String esp_cam_get_base64_jpeg();
void esp_cam_cleanup();
size_t esp_cam_get_frame_size();

#endif

