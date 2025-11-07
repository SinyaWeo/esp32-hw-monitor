# HW Monitor

A hardware monitoring display project for ESP32-C3 that shows CPU and GPU metrics on a ST7789 TFT display using LVGL graphics library.

![IMG_20251106_211236](https://github.com/user-attachments/assets/21f8f395-d08b-4f7e-91cd-56e46d6ed51a)

## Overview

Real-time hardware monitor with three circular gauge widgets displaying:
- **CPU Temperature** (left gauge)
- **CPU Load** (center gauge)
- **GPU Temperature** (right gauge)

The gauges feature color-coded zones that change based on the monitored values, providing quick visual feedback on system status.

## Hardware Requirements

- **ESP32-C3 SuperMini**
- **ST7789 TFT Display** (76x284 pixels)
- Serial connection to host PC for data input

### Pin Configuration (user defined)

| Function | GPIO Pin |
|----------|----------|
| TFT CS   | 10       |
| TFT RST  | 8        |
| TFT DC   | 9        |
| TFT MOSI | 7        |
| TFT SCLK | 6        |
| TFT BL   | 20       |

## Software Dependencies

- **PlatformIO** - Build system and IDE
- **Arduino Framework** - ESP32 Arduino core
- **LVGL** - Light and Versatile Graphics Library for embedded displays
- **LovyanGFX** - Fast graphics library for various display controllers

## Features

### Color-Coded Zones

#### Temperature Gauges (CPU & GPU) (user defined)
- 🔵 **Blue**: < 35°C (Cool)
- 🟢 **Green**: 35-55°C (Normal)
- 🟠 **Orange**: 55-75°C (Warm)
- 🔴 **Red**: 75-100°C (Hot)

#### Load Gauge (CPU)  (user defined)
- 🔵 **Blue**: < 40% (Low)
- 🟢 **Green**: 40-60% (Normal)
- 🟠 **Orange**: 60-80% (High)
- 🔴 **Red**: 80-100% (Critical)

### Display Layout
```
┌──────────────────────────────────────┐
│  [CPU Temp]  [CPU Load]  [GPU Temp]  │
│     °C          %           °C       │
└──────────────────────────────────────┘
```

## Serial Communication Protocol

The device expects data via serial port at **115200 baud** in the following format:

```
<cpu_temp>,<cpu_load>,<gpu_temp>\n
```

### Example
```
45,60,55
```
This would set:
- CPU Temperature: 45°C
- CPU Load: 60%
- GPU Temperature: 55°C

## Building and Uploading

### Using PlatformIO CLI

```bash
# Build the project
pio run

# Upload to device
pio run --target upload

# Monitor serial output
pio device monitor
```

### Using PlatformIO IDE (VS Code)

1. Open the project folder in VS Code
2. Use the PlatformIO toolbar:
   - Click "Build" to compile
   - Click "Upload" to flash the device
   - Click "Serial Monitor" to view output

## Configuration

### Display Configuration

The display is configured in `include/display.hpp` using LovyanGFX with the following settings:
- Resolution: 284x76 pixels
- SPI Mode: 0
- Write frequency: 40 MHz
- Read frequency: 16 MHz

### Scale Ranges

Modify these values in `src/main.cpp` to adjust gauge ranges:

```cpp
// Temperature scale
static int32_t scale_temperature_min = 20;
static int32_t scale_temperature_max = 100;

// Load scale
static int32_t scale_load_min = 0;
static int32_t scale_load_max = 100;
```

### Zone Thresholds

Adjust color zone transitions:

```cpp
// Temperature zones
static int32_t zone1_temperature =  35;
static int32_t zone2_temperature =  55;
static int32_t zone3_temperature =  75;
static int32_t zone4_temperature = 100;

// Load zones
static int32_t zone1_load =  40;
static int32_t zone2_load =  60;
static int32_t zone3_load =  80;
static int32_t zone4_load = 100;
```

## Project Structure

```
HW Monitor/
├── platformio.ini          # PlatformIO configuration
├── include/
│   ├── display.hpp         # LovyanGFX display driver configuration
│   ├── pin_config.h        # Pin definitions for TFT display
│   └── lv_conf.h          # LVGL configuration
├── src/
    └── main.cpp           # Main application code
```

## Host PC Integration

see python-daemon dir

## Troubleshooting

### Display not working
- Check pin connections match `pin_config.h`
- Verify backlight is connected to GPIO 20
- Ensure power supply is adequate (5V/1A minimum)

### No serial data received
- Verify baud rate is set to 115200
- Check USB cable supports data (not charge-only)
- Confirm correct COM port is selected

### Gauges not updating
- Check serial data format matches: `<int>,<int>,<int>\n`
- Ensure values are within configured ranges
- Monitor serial output for parsing errors

## License

This project is provided as-is for educational and personal use.

## Acknowledgments

- Built with [PlatformIO](https://platformio.org/)
- Graphics powered by [LVGL](https://lvgl.io/)
- Display driver by [LovyanGFX](https://github.com/lovyan03/LovyanGFX)
