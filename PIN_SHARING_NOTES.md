# Pin Sharing Notes for ESP32-CAM

## ⚠️ Important: Pin Sharing Strategy

Since ESP32-CAM has only **8 GPIO pins available**, we must share pins between components. This requires careful timing in the code.

## Pin Sharing Map

| GPIO | Components | Conflict Resolution |
|------|-----------|---------------------|
| **0** | Keyboard Row 4 | Boot pin - keep HIGH during boot |
| **2** | RFID SS, Keyboard Col 4 | Scan keyboard when RFID idle |
| **4** | RFID RST, Keyboard Col 3 | Scan keyboard when RFID idle |
| **12** | OLED SDA, Keyboard Col 2 | Scan keyboard when I2C idle |
| **13** | OLED SCL, Keyboard Col 1 | Scan keyboard when I2C idle |
| **14** | Keyboard Row 1 | Dedicated |
| **15** | Keyboard Row 2 | Dedicated |
| **16** | PIR Sensor, Keyboard Row 3 | Read PIR before keyboard scan |

## Timing Strategy

### 1. Keyboard Scanning
- **When to scan:** 
  - Between I2C display updates
  - When RFID is not reading cards
  - After reading PIR sensor
- **Frequency:** 50Hz (every 20ms)
- **Duration:** < 1ms per scan

### 2. I2C (OLED Display)
- **When to update:**
  - After keyboard scan completes
  - During state transitions
  - Not during RFID card reading
- **Duration:** ~5-10ms per update

### 3. RFID Card Reading
- **When to read:**
  - When keyboard is not being scanned
  - When display is not updating
- **Duration:** ~100-200ms per read

### 4. PIR Sensor
- **When to read:**
  - Before keyboard scanning
  - Every 100ms
- **Duration:** < 1ms

## Code Implementation

The current implementation handles pin sharing by:

1. **Non-blocking operations:** All operations are quick and non-blocking
2. **Sequential access:** Components are accessed one at a time
3. **State machine:** Ensures only one component uses shared pins at a time

## Potential Issues

### GPIO 0 (Boot Pin)
- **Issue:** If GPIO 0 is LOW during boot, ESP32 enters download mode
- **Solution:** Keep GPIO 0 HIGH during initialization
- **Risk:** Low - keyboard keeps rows HIGH when not scanning

### I2C + Keyboard Conflict
- **Issue:** GPIO 12/13 used for both I2C and keyboard
- **Solution:** 
  - Update display only during state transitions
  - Scan keyboard continuously but briefly
  - Use delays between I2C and keyboard operations

### RFID + Keyboard Conflict
- **Issue:** GPIO 2/4 used for both RFID and keyboard
- **Solution:**
  - RFID only active during card detection (brief)
  - Keyboard scanning pauses during RFID read
  - RFID operations are infrequent

### PIR + Keyboard Conflict
- **Issue:** GPIO 16 used for both PIR and keyboard Row 3
- **Solution:**
  - Read PIR before keyboard scan
  - Keyboard Row 3 disabled during PIR read (if needed)
  - PIR read is very brief (< 1ms)

## Testing Checklist

- [ ] Boot successfully (GPIO 0 HIGH)
- [ ] Keyboard all keys work
- [ ] OLED display updates correctly
- [ ] RFID reads cards
- [ ] PIR detects motion
- [ ] No conflicts during normal operation
- [ ] No conflicts during rapid state changes

## If Conflicts Occur

1. **Add delays** between operations using shared pins
2. **Reduce keyboard scan frequency** (from 50Hz to 20Hz)
3. **Use GPIO 0 for PIR** instead of GPIO 16 (if GPIO 0 available)
4. **Reduce keyboard to 3x4** (12 keys instead of 16) to free one pin

## Recommended Testing Order

1. Test each component individually
2. Test keyboard + OLED together
3. Test keyboard + RFID together
4. Test keyboard + PIR together
5. Test all components together
6. Test rapid state transitions
7. Test boot sequence multiple times

---

**Remember:** Pin sharing works because operations are brief and sequential. The state machine ensures only one component uses shared pins at a time.

