# IOTrix 2025: Tokenless Dining Management System

A complete ESP32-based dining management system that authenticates students via **face recognition + RFID dual verification**, prevents fraud, manages eligibility, and provides real-time manager approval interface.

## 🎯 Project Overview

This system eliminates the need for physical tokens by using:
- **ESP32-CAM** for face recognition
- **RFID RC522** for card-based identification
- **Dual verification** to prevent fraud
- **Manager approval interface** for edge cases
- **Offline mode** for network failures

## 🔧 Hardware Requirements

- ESP32-CAM (face recognition)
- RFID RC522 Module
- 16-Button Matrix Keyboard (4x4)
- 0.96" OLED Display (128x64, I2C)
- IR Motion Sensor (PIR)
- WiFi connectivity

## 📋 Pin Configuration

### RFID RC522 (SPI)
- MOSI: GPIO 23 (internal SPI)
- MISO: GPIO 19 (internal SPI)
- CLK: GPIO 18 (internal SPI)
- CS (SS): GPIO 2 ⚠️ (shared with Keyboard)
- RST: GPIO 4 ⚠️ (shared with Keyboard)

### OLED Display (I2C)
- SDA: GPIO 12 ⚠️ (shared with Keyboard)
- SCL: GPIO 13 ⚠️ (shared with Keyboard)
- Address: 0x3C

### 16-Button Matrix Keyboard
- Row Pins: GPIO 14, 15, 16, 0 ⚠️ (GPIO 0 is boot pin, GPIO 16 shared with PIR)
- Column Pins: GPIO 13, 12, 4, 2 ⚠️ (all shared with other components)

### IR Motion Sensor (PIR)
- Signal Pin: GPIO 16 ⚠️ (shared with Keyboard Row 3)

**⚠️ Note:** ESP32-CAM has only 8 GPIO pins available. Pins are shared between components. See `PIN_LAYOUT_ESP32CAM.md` for complete pin assignment details and sharing strategy.

### ESP32-CAM
- Uses internal camera interface
- Configured via camera_config_t

## 🚀 Getting Started

### 1. Install PlatformIO

```bash
# Install PlatformIO CLI or use PlatformIO IDE
pip install platformio
```

### 2. Clone and Build

```bash
cd Team_3_idiots
pio run -e esp32cam
```

### 3. Upload to ESP32

```bash
pio run -e esp32cam -t upload
```

### 4. Monitor Serial Output

```bash
pio device monitor
```

### 5. Configure System

Edit `src/main.cpp` and update:
- WiFi SSID and password
- Server IP address
- Server port (default: 5000)

Or use the web configuration portal (future feature).

## 📁 Project Structure

```
IOTrix_Dining_System/
├── src/
│   ├── main.cpp                    # Entry point
│   ├── app/
│   │   └── dining_system.cpp       # Main state machine
│   ├── modules/
│   │   ├── rfid_module.cpp         # RFID driver
│   │   ├── oled_display.cpp        # OLED driver
│   │   ├── keyboard_module.cpp     # Keyboard driver
│   │   ├── motion_sensor.cpp       # Motion sensor driver
│   │   └── esp_cam_module.cpp      # Camera driver
│   ├── services/
│   │   ├── face_recognition_service.cpp
│   │   ├── fraud_detection.cpp    # Fraud rules engine
│   │   └── api_client.cpp         # HTTP API client
│   ├── storage/
│   │   └── transaction_cache.cpp  # Local transaction storage
│   ├── ui/
│   │   └── manager_approval.cpp   # Manager approval UI
│   ├── power_management.cpp       # Sleep/wake logic
│   ├── config/
│   │   ├── data_types.h           # Core data structures
│   │   └── config.h               # Configuration management
│   └── utils/
│       ├── logger.cpp              # Logging utilities
│       └── helpers.cpp             # Helper functions
├── platformio.ini                  # PlatformIO configuration
└── README.md
```

## 🔄 System Flow

```
Student Approaches
    ↓
[IR Sensor] → Detects presence → Wake system
    ↓
[OLED Display] → Show "Scan Card"
    ↓
[RFID Module] → Read student ID from card
    ↓
[ESP-CAM] → Capture face image
    ↓
[Backend API Call] → Verify identity + eligibility
    ↓
[Fraud Detection] → Check rules
    ↓
[Decision]
    ├─→ APPROVED → Serve meal
    ├─→ DENIED → Show error
    └─→ MANAGER_APPROVAL → Wait for button input
    ↓
[Transaction Log] → Store locally + sync to cloud
    ↓
Auto-sleep after 30 seconds
```

## 🛡️ Fraud Detection Rules

1. **Double-Serving Rule**: Same student_id appears twice in last 6 hours → DENY
2. **Balance Rule**: Balance ≤ 0 → DENY
3. **Face Confidence Rule**: Confidence < 0.80 → REQUIRE MANAGER APPROVAL
4. **Already Served**: Already served today → DENY
5. **Meal Plan Expired**: Check against active meal plans → DENY
6. **Rapid Multiple Attempts**: 3+ failed attempts in 10 min → LOCK (require manager)
7. **Eligibility Check**: Student not eligible → DENY

## ⌨️ Keyboard Layout

```
[1] [2] [3] [APPROVE]
[4] [5] [6] [DENY]
[7] [8] [9] [RETRY]
[*] [0] [#] [OVERRIDE]
```

## 🔌 API Endpoints

The system expects a backend API with the following endpoints:

### POST `/api/face/verify`
Verify face + RFID combination.

**Request:**
```json
{
  "rfid_uid": "1234567890",
  "face_image": "base64_jpeg_string",
  "timestamp": 1234567890
}
```

**Response:**
```json
{
  "status": "success",
  "student_id": "12345",
  "student_name": "Piyal Chakraborty",
  "confidence": 0.95,
  "eligible": true,
  "balance": 250.50,
  "meal_plan": "active",
  "already_served_today": false,
  "approval_required": false,
  "reason": "Auto-approved - matched credentials"
}
```

### POST `/api/transactions/log`
Log a transaction.

**Request:**
```json
{
  "timestamp": 1234567890,
  "student_id": "12345",
  "student_name": "Piyal Chakraborty",
  "rfid_uid": "1234567890",
  "status": "approved",
  "balance_before": 250.50,
  "balance_after": 245.50,
  "reason": "Standard serving",
  "fraud_alert": false,
  "face_confidence": 0.95
}
```

## 🔋 Power Management

- **Auto-sleep**: System enters sleep mode after 30 seconds of no motion
- **Wake on motion**: PIR sensor wakes system when motion detected
- **CPU frequency**: Reduced to 80MHz during sleep, 240MHz when active
- **OLED**: Turned off during sleep

## 📊 State Machine

```
IDLE
  ↓ [Motion detected]
WAITING_FOR_CARD
  ↓ [Card detected]
CAPTURING_FACE
  ↓ [Face captured]
VERIFYING
  ↓ [Verification complete]
DECISION
  ├─→ APPROVED
  ├─→ DENIED
  └─→ MANAGER_APPROVAL_WAIT
  ↓ [Decision made]
TRANSACTION_LOG
  ↓ [Timer]
IDLE
```

## 🐛 Debugging

Enable debug logging by ensuring `DEBUG` is defined in `src/utils/logger.cpp`:

```cpp
#define DEBUG 1
```

All state transitions, API calls, and errors are logged to Serial at 115200 baud.

## 📝 Configuration

WiFi credentials are configured in `src/main.cpp`:

```cpp
config.wifi_ssid = "BDSET";
config.wifi_password = "Bdset@1234";
config.server_ip = "192.168.1.100";  // Update with your backend server IP
config.server_port = 5000;
config.motion_timeout_sec = 30;
config.min_face_confidence = 0.80;
config.offline_mode_enabled = true;
```

Note: Credentials are automatically saved to SPIFFS on first boot. To change them, edit `src/main.cpp` and re-upload.

## 🔧 Dependencies

All dependencies are managed via PlatformIO and listed in `platformio.ini`:

- **U8G2**: OLED display library
- **MFRC522**: RFID library
- **ArduinoJson**: JSON parsing
- **ESPAsyncWebServer**: Web server (for future config portal)
- **ESP32 Camera**: Camera driver

## 🚨 Error Handling

### Offline Mode
- If network fails, system uses cached data for eligibility checks
- Transactions are logged locally and synced when online
- Display shows "OFFLINE MODE - Limited verification"

### Hardware Failures
- **RFID fails**: Try face-only (less secure but graceful)
- **Face capture fails**: Try RFID-only + manual approval
- **OLED fails**: Log to Serial for debugging
- **Motion sensor fails**: Keep system awake

## ✅ Success Criteria

### Must Work:
- ✅ Face recognition detects/identifies student
- ✅ RFID reads card UID correctly
- ✅ Double-serving prevention works
- ✅ Manager approval screen with button input
- ✅ Transaction logs persistently
- ✅ Graceful offline operation

### Should Work:
- ✅ Auto-sleep on no motion
- ✅ Transaction audit trail
- ✅ Anti-spoofing measures
- ⏳ Web config portal (future)

## 📄 License

This project is developed for IOTrix 2025 hackathon.

## 👥 Team

Team 3 Idiots - Piyal's Zero-Token Crisis Solution

---

**Built with ❤️ for IOTrix 2025**

