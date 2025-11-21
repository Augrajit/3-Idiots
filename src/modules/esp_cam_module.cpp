#include "esp_cam_module.h"
#include "esp_camera.h"
#include "../utils/logger.h"
#include "../utils/helpers.h"

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

bool cam_initialized = false;
camera_fb_t* fb = nullptr;

bool esp_cam_init() {
  if (cam_initialized) {
    return true;
  }
  
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // Frame size and quality
  if(psramFound()){
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 80;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 80;
    config.fb_count = 1;
  }
  
  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Logger::logError("Camera: Init failed with error 0x" + String(err, HEX));
    return false;
  }
  
  cam_initialized = true;
  Logger::logInfo("ESP-CAM: Initialized successfully");
  return true;
}

bool esp_cam_capture_frame() {
  if (!cam_initialized) {
    return false;
  }
  
  // Free previous frame if exists
  if (fb != nullptr) {
    esp_camera_fb_return(fb);
    fb = nullptr;
  }
  
  // Capture frame
  fb = esp_camera_fb_get();
  if (!fb) {
    Logger::logError("Camera: Capture failed");
    return false;
  }
  
  Logger::logInfo("Camera: Frame captured (" + String(fb->len) + " bytes)");
  return true;
}

String esp_cam_get_base64_jpeg() {
  if (fb == nullptr || fb->buf == nullptr) {
    return "";
  }
  
  return Helpers::base64Encode(fb->buf, fb->len);
}

void esp_cam_cleanup() {
  if (fb != nullptr) {
    esp_camera_fb_return(fb);
    fb = nullptr;
  }
}

size_t esp_cam_get_frame_size() {
  if (fb == nullptr) {
    return 0;
  }
  return fb->len;
}

