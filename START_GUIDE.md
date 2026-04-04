# 🚀 Smart Clock — Start Guide

Follow these steps to build the project and launch the Web Flasher on your computer.

### 1 — Build the Firmware
Compile the source code into binary files for the ESP32-C3.
```bash
./build.sh build
```

### 2 — Prepare the Web App
Copy the compiled binaries into the `web/` folder.
```bash
./build.sh web
```

### 3 — Launch the Flasher
Start a local server to host the Web Flasher interface.
```bash
cd web
python -m http.server 8080
```
*Note: Use `python` on Windows or `python3` on macOS/Linux.*

### 4 — Open in Browser
Visit the following URL to start flashing your device:
**[http://localhost:8080](http://localhost:8080)**

---

## 🛠️ Project Shortcuts
- **Clean Build**: `pio run -t clean`
- **Serial Monitor**: `pio device monitor -e esp32-c3-supermini`
- **Unified Build & Prep**: `bash build.sh build && bash build.sh web`
