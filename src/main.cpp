#include <Arduino.h>
#include <time.h>
#include "config.h"
#include "display.h"
#include "net.h"
#include <WiFi.h>

// ── Globals ──────────────────────────────────────────────────────────────────
DisplayManager display;
NetManager     net;

// Boot-mode button: hold BOOT (GPIO9 on C3) at startup → clear WiFi credentials
#define BOOT_PIN   9

// ── State ────────────────────────────────────────────────────────────────────
uint32_t lastClockUpdate  = 0;
uint32_t lastActivity     = 0;
bool     ntpOK            = false;
bool     firstDraw        = true;

// ─────────────────────────────────────────────────────────────────────────────
//  OTA progress callback (called from net.cpp via extern)
// ─────────────────────────────────────────────────────────────────────────────
void displayOTAProgress(int pct) {
  display.showOTAProgress(pct);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Face-cycle button (BOOT pin, short press)
// ─────────────────────────────────────────────────────────────────────────────
static bool     btnPrev     = HIGH;
static uint32_t btnPressAt  = 0;
static bool     longHandled = false;

void handleButton() {
  bool cur = digitalRead(BOOT_PIN);

  if (cur == LOW && btnPrev == HIGH) {
    // Pressed
    btnPressAt  = millis();
    longHandled = false;
    lastActivity = millis();
  }

  if (cur == LOW && !longHandled && (millis() - btnPressAt > 2000)) {
    // Long press → reset WiFi credentials
    longHandled = true;
    display.showMessage("WiFi reset!", "Rebooting...");
    delay(1500);
    WiFi.disconnect(true, true);
    delay(200);
    ESP.restart();
  }

  if (cur == HIGH && btnPrev == LOW) {
    // Released
    uint32_t held = millis() - btnPressAt;
    if (!longHandled && held < 2000) {
      // Short press → cycle face
      display.nextFace();
    }
  }

  btnPrev = cur;
}

// ─────────────────────────────────────────────────────────────────────────────
//  setup()
// ─────────────────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n\n=== Smart Clock v" FIRMWARE_VERSION " ===");

  // Button pin (shared with BOOT)
  pinMode(BOOT_PIN, INPUT_PULLUP);

  // OLED
  if (!display.begin()) {
    Serial.println("OLED init FAILED. Check SDA/SCL wiring.");
    // Blink internal LED to signal error
    pinMode(LED_PIN, OUTPUT);
    while (true) {
      digitalWrite(LED_PIN, LOW);  delay(200);
      digitalWrite(LED_PIN, HIGH); delay(200);
    }
  }

  display.showBootScreen();
  delay(1500);

  // WiFi
  bool connected = net.connect();

  if (connected) {
    display.showConnected(net.getIP().c_str());

    // NTP sync
    ntpOK = net.syncNTP();
    if (!ntpOK) {
      display.showError("NTP sync failed");
      delay(1500);
    }

    // Initial OTA check
    net.checkOTA();
  } else {
    display.showMessage("No WiFi", "Clock only mode");
    delay(2000);

    // Try to restore last known time from RTC (ESP32 deep sleep survives)
    // If no RTC time, set a placeholder
    struct timeval tv = { .tv_sec = 0, .tv_usec = 0 };
    settimeofday(&tv, nullptr);
  }

  lastActivity = millis();
  Serial.println("[MAIN] Setup complete. Entering clock loop.");
}

// ─────────────────────────────────────────────────────────────────────────────
//  loop()
// ─────────────────────────────────────────────────────────────────────────────
void loop() {
  handleButton();
  net.loop();

  uint32_t now = millis();

  // Screensaver / dim
  if (SCREENSAVER_MS > 0 && (now - lastActivity) > SCREENSAVER_MS) {
    display.setDimmed(true);
  } else {
    display.setDimmed(false);
  }

  // Clock redraw
  if (now - lastClockUpdate >= CLOCK_UPDATE_MS) {
    lastClockUpdate = now;

    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 0)) {
      // Valid time
      if (!ntpOK) ntpOK = true;
      display.update(&timeinfo);
    } else if (!ntpOK) {
      // No time yet; show waiting screen
      if (net.isConnected()) {
        display.showMessage("Syncing time...", NTP_SERVER1);
      } else {
        // Offline: show elapsed time since boot as HH:MM:SS (fallback)
        uint32_t sec = now / 1000;
        struct tm fake = {};
        fake.tm_hour = (sec / 3600) % 24;
        fake.tm_min  = (sec / 60)   % 60;
        fake.tm_sec  =  sec         % 60;
        fake.tm_mday = 1;
        fake.tm_wday = 0;
        display.update(&fake);
      }
    }
  }

  // Yield
  delay(10);
}
