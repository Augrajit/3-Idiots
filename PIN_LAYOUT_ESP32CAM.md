# ESP32-CAM AI-Thinker Pin Layout

## ESP32-CAM AI-Thinker Pinout Diagram

Based on the official ESP32-CAM AI-Thinker pinout, the board has **8 GPIO pins** available for external use.

### Left Side Pins (from top to bottom):
1. **5V** - Power (Red)
2. **GND** - Ground (Black)
3. **GPIO 12** (HS2_DATA2) - I/O (Blue)
4. **GPIO 13** (HS2_DATA3) - I/O (Blue)
5. **GPIO 15** (HS2_CMD) - I/O (Blue)
6. **GPIO 14** (HS2_CLK) - I/O (Blue)
7. **GPIO 2** (HS2_DATA0) - I/O (Blue)
8. **GPIO 4** (HS2_DATA1/FLASH) - I/O (Blue)

### Right Side Pins (from top to bottom):
1. **3.3V** - Power (Red)
2. **GPIO 16** (U2RXD) - I/O (Blue)
3. **GPIO 0** (CSI_MCLK) - I/O (Blue, Camera Clock)
4. **GND** - Ground (Black)
5. **3.3V/5V** - Power Output (Yellow)
6. **GPIO 3** (U0RXD) - Serial RX (Purple) - ⚠️ Avoid using
7. **GPIO 1** (U0TXD) - Serial TX (Purple) - ⚠️ Avoid using
8. **GND** - Ground (Black)

**Available GPIO Pins:** 0, 2, 4, 12, 13, 14, 15, 16 (8 pins total)

---

## Hardware Component Pin Assignments

### 1. RFID RC522 Module (SPI)

| Pin Name | ESP32 GPIO | Pin Location | Notes |
|----------|-----------|--------------|-------|
| **SS/CS** | **GPIO 2** | Left side pin 7 | HS2_DATA0, shared with Keyboard Col 4 |
| **RST** | **GPIO 4** | Left side pin 8 | HS2_DATA1/FLASH, shared with Keyboard Col 3 |
| **MOSI** | GPIO 23 | Internal | Camera pin (SPI default) |
| **MISO** | GPIO 19 | Internal | Camera pin (SPI default) |
| **SCK** | GPIO 18 | Internal | Camera pin (SPI default) |
| **3.3V** | 3.3V | Right side pin 1 | Power |
| **GND** | GND | Any GND pin | Ground |

**Total Pins Used: 2** (GPIO 2, 4)

---

### 2. OLED Display 0.96" (I2C)

| Pin Name | ESP32 GPIO | Pin Location | Notes |
|----------|-----------|--------------|-------|
| **SDA** | **GPIO 12** | Left side pin 3 | HS2_DATA2, shared with Keyboard Col 2 |
| **SCL** | **GPIO 13** | Left side pin 4 | HS2_DATA3, shared with Keyboard Col 1 |
| **VCC** | 3.3V | Right side pin 1 | Power |
| **GND** | GND | Any GND pin | Ground |

**I2C Address:** `0x3C`

**Total Pins Used: 2** (GPIO 12, 13)

**⚠️ Note:** GPIO 12 and 13 are shared with keyboard columns. I2C operations must be timed to avoid conflicts with keyboard scanning.

---

### 3. 16-Button Matrix Keyboard (4x4)

#### Row Pins (Outputs):
| Row | ESP32 GPIO | Pin Location | Notes |
|-----|-----------|--------------|-------|
| Row 1 | **GPIO 14** | Left side pin 6 | HS2_CLK, dedicated |
| Row 2 | **GPIO 15** | Left side pin 5 | HS2_CMD, dedicated |
| Row 3 | **GPIO 16** | Right side pin 2 | U2RXD, shared with PIR |
| Row 4 | **GPIO 0** | Right side pin 3 | CSI_MCLK (camera clock), can be used as GPIO |

#### Column Pins (Inputs with Pull-up):
| Column | ESP32 GPIO | Pin Location | Notes |
|--------|-----------|--------------|-------|
| Col 1 | **GPIO 13** | Left side pin 4 | Shared with OLED SCL |
| Col 2 | **GPIO 12** | Left side pin 3 | Shared with OLED SDA |
| Col 3 | **GPIO 4** | Left side pin 8 | Shared with RFID RST |
| Col 4 | **GPIO 2** | Left side pin 7 | Shared with RFID SS |

**Total Pins Used: 8** (all available pins, with sharing)

**⚠️ Important Pin Sharing:**
- All column pins are shared with other components
- Row 3 (GPIO 16) is shared with PIR sensor
- Row 4 (GPIO 0) is camera clock - keep HIGH during boot

#### Key Layout:
```
[1]    [2]    [3]    [APPROVE]
[4]    [5]    [6]    [DENY]
[7]    [8]    [9]    [RETRY]
[*]    [0]    [#]    [OVERRIDE]
```

---

### 4. IR Motion Sensor (PIR)

| Pin Name | ESP32 GPIO | Pin Location | Notes |
|----------|-----------|--------------|-------|
| **Signal** | **GPIO 16** | Right side pin 2 | U2RXD, shared with Keyboard Row 3 |
| **VCC** | 5V | Left side pin 1 | Power (5V) |
| **GND** | GND | Any GND pin | Ground |

**Total Pins Used: 1** (GPIO 16, shared with keyboard)

**⚠️ Note:** GPIO 16 is shared with keyboard Row 3. Read PIR before keyboard scanning to avoid conflicts.

---

### 5. ESP32-CAM (Built-in Camera)

The camera uses internal pins (not available for external use):
- Camera interface: GPIO 0, 4, 5, 12, 13, 14, 15, 16, 18, 19, 21, 22, 23, 25, 26, 27, 32, 34, 35, 36, 39
- **No external connections needed** - camera is built into the module

---

## Complete Pin Usage Map

| GPIO | Pin Location | Component(s) | Secondary Function | Sharing |
|------|-------------|-------------|-------------------|---------|
| **0** | Right pin 3 | Keyboard Row 4 | CSI_MCLK (Camera Clock) | None |
| **2** | Left pin 7 | RFID SS, Keyboard Col 4 | HS2_DATA0 | Shared |
| **4** | Left pin 8 | RFID RST, Keyboard Col 3 | HS2_DATA1/FLASH | Shared |
| **12** | Left pin 3 | OLED SDA, Keyboard Col 2 | HS2_DATA2 | Shared |
| **13** | Left pin 4 | OLED SCL, Keyboard Col 1 | HS2_DATA3 | Shared |
| **14** | Left pin 6 | Keyboard Row 1 | HS2_CLK | Dedicated |
| **15** | Left pin 5 | Keyboard Row 2 | HS2_CMD | Dedicated |
| **16** | Right pin 2 | PIR Sensor, Keyboard Row 3 | U2RXD | Shared |

**Total Pins Used: 8** (all available GPIO pins)

---

## Power Connections

| Component | Voltage | Pin Source |
|-----------|---------|------------|
| ESP32-CAM | 5V | Left side pin 1 (5V) |
| RFID RC522 | 3.3V | Right side pin 1 (3.3V) |
| OLED Display | 3.3V | Right side pin 1 (3.3V) |
| Matrix Keyboard | 3.3V | Right side pin 1 (3.3V) |
| PIR Sensor | 5V | Left side pin 1 (5V) |

**All GND connections:** Use any GND pin (Left pin 2, Right pin 4, or Right pin 8)

---

## Pin Sharing Strategy

### Timing Requirements:

1. **I2C (OLED) + Keyboard:**
   - Update display only during state transitions
   - Scan keyboard when I2C is idle
   - Minimum 10ms delay between I2C and keyboard operations

2. **RFID + Keyboard:**
   - RFID SS (GPIO 2) active only during card reading (~100-200ms)
   - Scan keyboard when RFID is idle
   - RFID operations are infrequent

3. **PIR + Keyboard:**
   - Read PIR sensor before keyboard scanning
   - PIR read is very brief (< 1ms)
   - Keyboard Row 3 can be disabled during PIR read if needed

4. **GPIO 0 (Camera Clock):**
   - Used for camera MCLK but can function as GPIO
   - Keep HIGH during boot to prevent download mode
   - Safe to use after system initialization

---

## Connection Checklist

### Power Connections:
- [ ] ESP32-CAM 5V → Left side pin 1 (5V)
- [ ] RFID VCC → Right side pin 1 (3.3V)
- [ ] OLED VCC → Right side pin 1 (3.3V)
- [ ] Keyboard VCC → Right side pin 1 (3.3V)
- [ ] PIR VCC → Left side pin 1 (5V)
- [ ] All GND → Common GND (any GND pin)

### Signal Connections:
- [ ] RFID SS → Left side pin 7 (GPIO 2)
- [ ] RFID RST → Left side pin 8 (GPIO 4)
- [ ] RFID MOSI → Internal (GPIO 23)
- [ ] RFID MISO → Internal (GPIO 19)
- [ ] RFID SCK → Internal (GPIO 18)
- [ ] OLED SDA → Left side pin 3 (GPIO 12)
- [ ] OLED SCL → Left side pin 4 (GPIO 13)
- [ ] Keyboard Row 1 → Left side pin 6 (GPIO 14)
- [ ] Keyboard Row 2 → Left side pin 5 (GPIO 15)
- [ ] Keyboard Row 3 → Right side pin 2 (GPIO 16)
- [ ] Keyboard Row 4 → Right side pin 3 (GPIO 0)
- [ ] Keyboard Col 1 → Left side pin 4 (GPIO 13)
- [ ] Keyboard Col 2 → Left side pin 3 (GPIO 12)
- [ ] Keyboard Col 3 → Left side pin 8 (GPIO 4)
- [ ] Keyboard Col 4 → Left side pin 7 (GPIO 2)
- [ ] PIR Signal → Right side pin 2 (GPIO 16)

---

## Important Notes

1. **GPIO 0 (CSI_MCLK):**
   - Used by camera as master clock
   - Can be used as GPIO when camera is not actively capturing
   - Keep HIGH during boot to prevent download mode

2. **GPIO 4 (FLASH):**
   - Can control camera flash LED
   - Safe to use as GPIO for RFID RST

3. **GPIO 1 and GPIO 3:**
   - Serial communication pins (U0TXD, U0RXD)
   - **DO NOT USE** - reserved for Serial communication

4. **Pin Sharing:**
   - All pins except GPIO 14 and 15 are shared
   - Code handles sharing with sequential access
   - Test each component individually first

---

## Testing Order

1. ✅ Test ESP32-CAM boot (GPIO 0 HIGH)
2. ✅ Test OLED display (GPIO 12, 13)
3. ✅ Test RFID module (GPIO 2, 4)
4. ✅ Test PIR sensor (GPIO 16)
5. ✅ Test keyboard individually (all pins)
6. ✅ Test all components together
7. ✅ Test rapid state transitions
8. ✅ Test boot sequence multiple times

---

**Reference:** ESP32-CAM AI-Thinker Pinout Diagram (RandomNerdTutorials.com)
