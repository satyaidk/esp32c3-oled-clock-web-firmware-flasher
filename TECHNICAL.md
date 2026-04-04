# ESP32-C3 Smart Clock — Full Technical Documentation

This document provides a comprehensive technical breakdown of the Smart Clock project, covering the hardware architecture, software stack, logic flows, and development lifecycle.

---

## 🏗️ Tech Stack

The project is built on a modern IoT stack optimized for the RISC-V based ESP32-C3 microcontroller.

### Core Hardware
- **MCU**: [ESP32-C3 Super Mini](https://www.wemos.cc/en/latest/c3/c3_mini.html) (RISC-V Single-Core @ 160MHz)
- **Display**: SSD1306 0.96" OLED (128×64 pixels, I²C)
- **Input**: Physical BOOT button (GPIO 9) used for UI interaction.

### Software Architecture
- **Language**: C++11 (Arduino Framework)
- **Build System**: [PlatformIO Core](https://platformio.org/)
- **Protocol**: I²C (Display), NTP (Time Sync), HTTPS (OTA Updates), Web Serial (Web Flasher)

---

## 📚 Library Dependency Analysis

The project leverages several high-quality open-source libraries, managed via `platformio.ini`:

| Library | Version | Purpose |
| :--- | :--- | :--- |
| **Adafruit SSD1306** | `^2.5.9` | Low-level driver for the OLED hardware. |
| **Adafruit GFX** | `^1.11.9` | Graphics primitive library (lines, circles, text). |
| **Adafruit BusIO** | `^1.15.0` | Unified I2C/SPI abstraction used by Adafruit drivers. |
| **ArduinoJson** | `^7.0.4` | Parsing remote `version.json` for OTA update checks. |
| **WiFiManager** | `^2.0.17` | Captive portal for credential setup and persistence in NVS. |

---

## ⚡ Hardware Architecture & Wiring

The ESP32-C3 Super Mini communicates with the OLED via the I²C bus.

### Pin Mapping
| ESP32-C3 Pin | SSD1306 Pin | Function | Notes |
| :--- | :--- | :--- | :--- |
| **3.3V** | VCC | Power | Main power rail. |
| **GND** | GND | Ground | |
| **GPIO 20** | SDA | Data | I²C Serial Data. |
| **GPIO 21** | SCL | Clock | I²C Serial Clock. |

### Button Multiplexing
GPIO 9 is unique as it is connected to the physical **BOOT button** on the Super Mini. The firmware uses an `INPUT_PULLUP` configuration to detect presses for:
- **Short Press**: Cycle clock faces.
- **Hold 2s**: Trigger a factory reset of WiFi credentials.

---

## 💻 Software Module Deep-Dive

### 1. `Main Loop (main.cpp)`
The central orchestrator. It manages the state machine:
- **Setup Phase**: Initializes Serial, OLED, and triggers `NetManager::connect()`.
- **Loop Phase**: 
  - Polls the button state.
  - Updates the `DisplayManager` every 500ms.
  - Keeps `NetManager` alive for reconnection and OTA timers.
- **Power Savings**: Implements a `SCREENSAVER_MS` timeout that dims the OLED contrast to prevent burn-in.

### 2. `Display Manager (display.cpp)`
A high-level abstraction for the OLED. It contains three distinct rendering engines:
- **Digital Face**: Focuses on readability with large fonts and a smooth seconds progress bar.
- **Analog Face**: Uses trigonometry (`sin`/`cos`) to render clock hands and tick marks.
- **Minimal Face**: A modern, bold take on time with a dot-matrix second progress row.
- **Status Screens**: Dedicated methods for WiFi Setup, OTA Progress, and Boot animations.

### 3. `Net Manager (net.cpp)`
Handles all external connectivity:
- **Captive Portal**: If no WiFi is found, it hosts an AP (`SmartClock-Setup`) and serves a web form to capture SSID, Password, and a custom **Timezone Offset** (stored in NVS via `Preferences.h`).
- **NTP Sync**: Connects to global time servers to fetch UTC time and applies the saved offset.
- **OTA Updates**: Polls a remote HTTPS endpoint for a `version.json`. If a higher version is found, it downloads the new `firmware.bin` and applies it automatically while showing a progress bar on the OLED.

---

## 🛠️ Development & Build Workflow

### 1. Build Environment
The project uses `platformio.ini` to define the environment:
- **Env**: `esp32-c3-supermini`
- **Board**: `esp32-c3-devkitm-1` (generic C3 profile)
- **Framework**: `arduino`

### 2. Automation (`build.sh`)
A unified bash script simplifies the development lifecycle:
```bash
./build.sh build    # Compiles code using PlatformIO
./build.sh flash    # Uploads to connected device
./build.sh web      # Bundles binaries for the Web Flasher
```

### 3. Web Flasher Distribution
The `web/` directory contains a static site using **ESP Web Tools**. 
- It uses the **Web Serial API** to allow users to flash the clock directly from Chrome or Edge.
- `manifest.json` tells the browser which offsets to flash the `firmware.bin`, `bootloader.bin`, and `partitions.bin`.

---

## 📈 Step-by-Step Deployment Process

1.  **Clone & Install**: Install PlatformIO Core and clone the project.
2.  **Compilation**: Run `./build.sh build` to fetch libraries and compile.
3.  **Local Serving**: Run `python -m http.server 8080` in the `web/` dir.
4.  **Flashing**: Open the browser, connect the ESP32-C3, and click "Flash".
5.  **Provisioning**: Connect to the clock's WiFi AP, set your credentials and timezone offset (e.g., `19800` for IST).
6.  **Success**: The clock syncs and begins operations.

---

## 🏥 Maintenance & Troubleshooting

| Symptom | Cause | Solution |
| :--- | :--- | :--- |
| **Blank Display** | Wrong I2C Pins | Verify SDA=8, SCL=9 in `config.h`. |
| **Wrong Time** | TZ Offset | Hold BOOT for 2s to reset and re-enter offset. |
| **OTA Failed** | SSL/URL issue | Check `OTA_VERSION_URL` is reachable and has a valid cert. |
| **Stuck in Setup** | Missing dependencies | Run `pio pkg install` to ensure all libs are present. |
