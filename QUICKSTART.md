# Quick Start Guide

## Hardware Setup

1. **Connect RFID RC522**:
   - MOSI → GPIO 23
   - MISO → GPIO 19
   - CLK → GPIO 18
   - CS → GPIO 5
   - RST → GPIO 17

2. **Connect OLED Display**:
   - SDA → GPIO 21
   - SCL → GPIO 22
   - VCC → 3.3V
   - GND → GND

3. **Connect Matrix Keyboard**:
   - Rows: GPIO 12, 14, 27, 26
   - Columns: GPIO 33, 32, 4, 2

4. **Connect PIR Sensor**:
   - Signal → GPIO 25
   - VCC → 5V
   - GND → GND

5. **ESP32-CAM**: Uses internal camera (no external connections needed)

## Software Setup

1. **Install PlatformIO**:
   ```bash
   pip install platformio
   ```

2. **Configure WiFi and Server**:
   Edit `src/main.cpp`:
   ```cpp
   config.wifi_ssid = "YourWiFiName";
   config.wifi_password = "YourPassword";
   config.server_ip = "192.168.1.100";  // Your backend server IP
   ```

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

