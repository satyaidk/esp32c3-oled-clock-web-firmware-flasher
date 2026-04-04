# 🕐 ESP32-C3 Smart Clock — Web Firmware Flasher

A full-featured smart clock for the **ESP32-C3 Super Mini** with a **SSD1306 128×64 OLED** display. Features NTP time sync, captive-portal WiFi setup, OTA firmware updates, and three animated clock faces.

This repository includes the firmware source and a binary distribution for the **Web Serial Flasher**.

---

## 🚀 Quick Start — Flash Now!
No software installation is required to flash your clock.
1.  Visit the **[Web Flasher](http://localhost:8080)** (hosted via Python).
2.  Connect your ESP32-C3 Super Mini to your computer via USB.
3.  Click **Connect** and select your device to flash the firmware.

---

## 🛠️ Hardware & Wiring
| Part | Notes |
| :--- | :--- |
| **ESP32-C3 Super Mini** | Low-power RISC-V SoC with USB-C. |
| **SSD1306 OLED (128×64)** | I²C 0.96" display module. |

### Wiring Diagram
| ESP32-C3 Pin | OLED Pin | Function |
| :--- | :--- | :--- |
| **3.3V** | VCC | Power |
| **GND** | GND | Ground |
| **GPIO 20** | SDA | I²C Serial Data |
| **GPIO 21** | SCL | I²C Serial Clock |

> [!NOTE]
> **GPIO 9** is the physical **BOOT button** on the Super Mini. It is used as the face-cycle / WiFi-reset button in our firmware.

---

## 📦 Installation & Setup Guide

### 1 — Prerequisites
- **PlatformIO**: Install the [PlatformIO IDE](https://platformio.org/platformio-ide) for VS Code or the CLI via `pip install platformio`.
- **Python**: Required for the local Web Flasher server.

### 2 — Build & Compile
```bash
# Clone the repository
git clone https://github.com/satyaidk/esp32c3-oled-clock-web-firmware-flasher.git
cd esp32c3-oled-clock-web-firmware-flasher

# Build the firmware
./build.sh build
```

### 3 — Launch the Web Flasher
To host the flasher UI locally:
```bash
./build.sh web
cd web
python -m http.server 8080
```
Then open `http://localhost:8080` in Chrome or Edge.

---

## ⚙️ Configuration
All hardware and networking settings are in `include/config.h`:
```cpp
#define OLED_SDA        20      // I²C SDA pin
#define OLED_SCL        21      // I²C SCL pin
#define FIRMWARE_VERSION "1.0.0"
#define SCREENSAVER_MS   60000  // Dim after 60s
```

---

## 📖 Further Documentation
- **[Technical Architecture](TECHNICAL.md)**: Deep-dive into libraries, OTA, and networking logic.
- **[Project Structure](PROJECT_STRUCTURE.md)**: Detailed mapping of every folder and file.
- **[Step-by-Step Guide](START_GUIDE.md)**: Detailed commands for building and flashing.

---

## 🤝 Contributing
Feel free to fork this project and submit pull requests. For major changes, please open an issue first.

## 📄 License
MIT — See [LICENSE](LICENSE) (if applicable) for details.
