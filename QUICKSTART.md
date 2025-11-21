# Quick Start Guide

## Hardware Setup

1. **Connect RFID RC522**:
   - MOSI → GPIO 23 (internal SPI)
   - MISO → GPIO 19 (internal SPI)
   - CLK → GPIO 18 (internal SPI)
   - CS → GPIO 2 ⚠️ (shared with Keyboard Col 4)
   - RST → GPIO 4 ⚠️ (shared with Keyboard Col 3)
   - VCC → 3.3V
   - GND → GND

2. **Connect OLED Display**:
   - SDA → GPIO 12 ⚠️ (shared with Keyboard Col 2)
   - SCL → GPIO 13 ⚠️ (shared with Keyboard Col 1)
   - VCC → 3.3V
   - GND → GND

3. **Connect Matrix Keyboard**:
   - Rows: GPIO 14, 15, 16, 0 ⚠️ (GPIO 0 is boot pin, GPIO 16 shared with PIR)
   - Columns: GPIO 13, 12, 4, 2 ⚠️ (all shared with other components)

4. **Connect PIR Sensor**:
   - Signal → GPIO 16 ⚠️ (shared with Keyboard Row 3)
   - VCC → 5V
   - GND → GND

**⚠️ IMPORTANT:** ESP32-CAM has only 8 GPIO pins available. Pins are shared between components. See PIN_LAYOUT_ESP32CAM.md for details.

5. **ESP32-CAM**: Uses internal camera (no external connections needed)

## Software Setup

1. **Install PlatformIO**:
   ```bash
   pip install platformio
   ```

2. **Configure WiFi and Server**:
   WiFi credentials are already configured in `src/main.cpp`:
   ```cpp
   config.wifi_ssid = "BDSET";
   config.wifi_password = "Bdset@1234";
   config.server_ip = "192.168.1.100";  // Your backend server IP
   ```
   
   Note: WiFi credentials are automatically saved to SPIFFS on first boot.

3. **Build and Upload**:
   ```bash
   pio run -e esp32cam -t upload
   ```

4. **Monitor Output**:
   ```bash
   pio device monitor
   ```

## Testing Flow

1. **Power on** → System initializes, shows "System Ready" on OLED
2. **Wave hand** in front of PIR sensor → System wakes, shows "Scan Card"
3. **Tap RFID card** → System reads UID, shows "Capturing Face"
4. **Face camera** → System captures image, shows "Verifying"
5. **Wait for decision**:
   - If approved → Shows "APPROVED" with balance
   - If denied → Shows error message
   - If needs approval → Shows manager screen, wait for button press

## Keyboard Testing

- Press **APPROVE** button → Approves transaction
- Press **DENY** button → Denies transaction
- Press **OVERRIDE** button → Forces approval (with warning)

## Troubleshooting

### RFID not reading
- Check SPI connections
- Verify card is MIFARE compatible
- Check Serial output for RFID errors

### Camera not working
- Ensure ESP32-CAM has PSRAM
- Check camera initialization in Serial output
- Verify sufficient lighting

### WiFi not connecting
- Check SSID and password
- Verify router is 2.4GHz (ESP32 doesn't support 5GHz)
- Check Serial output for connection status

### OLED blank
- Check I2C connections
- Verify address is 0x3C
- Check power supply (3.3V)

## Backend API Requirements

Your backend must implement:

1. **POST /api/face/verify** - Face + RFID verification
2. **POST /api/transactions/log** - Transaction logging

See README.md for API specifications.

## Offline Mode

If WiFi is unavailable, system operates in offline mode:
- Uses cached transaction data
- Basic eligibility checks only
- Transactions queued for sync when online

