#pragma once

// ─────────────────────────────────────────────
//  Hardware — ESP32-C3 Super Mini + SSD1306 OLED
// ─────────────────────────────────────────────
#define OLED_SDA        20     // GPIO20  → SDA
#define OLED_SCL        21     // GPIO21  → SCL
#define OLED_WIDTH      128
#define OLED_HEIGHT     64
#define OLED_ADDR       0x3C   // Default I2C address (some boards use 0x3D)
#define OLED_RESET      -1     // No reset pin

// Onboard LED (active LOW on C3 Super Mini)
#define LED_PIN         8      // Shared with SDA on some boards; set -1 to disable
#define LED_ACTIVE_LOW  true

// ─────────────────────────────────────────────
//  WiFi / NTP
// ─────────────────────────────────────────────
#define NTP_SERVER1     "pool.ntp.org"
#define NTP_SERVER2     "time.nist.gov"
#define NTP_SERVER3     "time.google.com"

// Default timezone offset (seconds). Overridden by WiFiManager portal setting.
// IST = UTC+5:30 → 19800. Change for your region:
//   UTC       = 0
//   EST (US)  = -18000  (UTC-5)
//   PST (US)  = -28800  (UTC-8)
//   CET (EU)  = 3600    (UTC+1)
//   IST (IN)  = 19800   (UTC+5:30)
//   JST (JP)  = 32400   (UTC+9)
#define TZ_OFFSET_SEC   19800
#define DST_OFFSET_SEC  0

// ─────────────────────────────────────────────
//  OTA / Update server
// ─────────────────────────────────────────────
#define FIRMWARE_VERSION    "1.0.0"
#define OTA_VERSION_URL     "https://your-domain.com/clock/version.json"
#define OTA_CHECK_INTERVAL  (6UL * 60 * 60 * 1000)   // 6 hours in ms

// ─────────────────────────────────────────────
//  WiFiManager AP
// ─────────────────────────────────────────────
#define AP_NAME         "SmartClock-Setup"
#define AP_PASSWORD     "clocksetup"
#define PORTAL_TIMEOUT  180    // seconds before AP times out

// ─────────────────────────────────────────────
//  Display timing
// ─────────────────────────────────────────────
#define CLOCK_UPDATE_MS     500    // redraw interval
#define SCREENSAVER_MS      60000  // dim after 60 s (set 0 to disable)
#define SCROLL_SPEED_MS     80     // ms per step for scrolling text
