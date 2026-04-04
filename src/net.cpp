#include "net.h"
#include "config.h"
#include <WiFi.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <ArduinoJson.h>
#include <time.h>

// ── Custom parameter persisted across reboots via Preferences ──────────────
#include <Preferences.h>
static Preferences prefs;

// External display reference (forward declared; set from main)
extern void displayOTAProgress(int pct);

// ─────────────────────────────────────────────────────────────────────────────
NetManager::NetManager() {}

// ─────────────────────────────────────────────────────────────────────────────
bool NetManager::connect() {
  WiFiManager wm;
  wm.setConfigPortalTimeout(PORTAL_TIMEOUT);
  wm.setConnectTimeout(20);
  wm.setConnectRetries(3);
  wm.setCleanConnect(true);

  // Custom timezone parameter stored in NVS
  prefs.begin("clock", false);
  int savedTZ = prefs.getInt("tz", TZ_OFFSET_SEC);
  char tzBuf[12];
  snprintf(tzBuf, sizeof(tzBuf), "%d", savedTZ);

  WiFiManagerParameter tzParam("tz", "Timezone offset (seconds from UTC)", tzBuf, 10);
  wm.addParameter(&tzParam);

  // Attempt auto-connect; if it fails, open portal
  Serial.println("[NET] Starting WiFiManager...");
  bool ok = wm.autoConnect(AP_NAME, AP_PASSWORD);

  if (ok) {
    _connected = true;
    Serial.printf("[NET] Connected! IP: %s\n", WiFi.localIP().toString().c_str());

    // Save TZ if changed
    int newTZ = String(tzParam.getValue()).toInt();
    if (newTZ != savedTZ) {
      prefs.putInt("tz", newTZ);
      Serial.printf("[NET] Saved TZ offset: %d s\n", newTZ);
    }
    prefs.end();

    // Apply timezone
    long tz = prefs.getInt("tz", TZ_OFFSET_SEC);
    configTime(tz, DST_OFFSET_SEC, NTP_SERVER1, NTP_SERVER2, NTP_SERVER3);
  } else {
    Serial.println("[NET] WiFiManager timed out. Running offline.");
    _connected = false;
    prefs.end();
  }

  return _connected;
}

// ─────────────────────────────────────────────────────────────────────────────
bool NetManager::syncNTP() {
  if (!_connected) return false;

  Serial.println("[NET] Waiting for NTP sync...");
  struct tm timeinfo;
  int retries = 20;
  while (!getLocalTime(&timeinfo, 1000) && retries-- > 0) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (retries <= 0) {
    Serial.println("[NET] NTP sync failed.");
    return false;
  }

  _ntpSynced = true;
  Serial.printf("[NET] Time synced: %02d:%02d:%02d\n",
                timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  return true;
}

// ─────────────────────────────────────────────────────────────────────────────
bool NetManager::checkOTA() {
  if (!_connected) return false;

  Serial.println("[OTA] Checking for update...");
  HTTPClient http;
  http.begin(OTA_VERSION_URL);
  http.setTimeout(8000);
  int code = http.GET();

  if (code != 200) {
    Serial.printf("[OTA] version.json fetch failed: HTTP %d\n", code);
    http.end();
    return false;
  }

  String body = http.getString();
  http.end();

  // Parse JSON: { "version": "1.0.1", "firmware_url": "https://..." }
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, body);
  if (err) {
    Serial.printf("[OTA] JSON parse error: %s\n", err.c_str());
    return false;
  }

  const char *remoteVer = doc["version"] | "";
  const char *fwUrl     = doc["firmware_url"] | "";

  if (String(remoteVer) == FIRMWARE_VERSION) {
    Serial.printf("[OTA] Up to date (v%s)\n", FIRMWARE_VERSION);
    return false;
  }

  Serial.printf("[OTA] New firmware: %s → %s\n", FIRMWARE_VERSION, remoteVer);
  Serial.printf("[OTA] Downloading: %s\n", fwUrl);

  return _fetchAndUpdate(String(fwUrl));
}

bool NetManager::_fetchAndUpdate(const String &url) {
  WiFiClient client;

  httpUpdate.setLedPin(LED_PIN, LOW);
  httpUpdate.rebootOnUpdate(true);

  httpUpdate.onProgress([](int cur, int total) {
    int pct = (total > 0) ? (cur * 100 / total) : 0;
    displayOTAProgress(pct);
    Serial.printf("[OTA] %d%%\n", pct);
  });

  t_httpUpdate_return ret = httpUpdate.update(client, url);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("[OTA] FAILED: %s\n", httpUpdate.getLastErrorString().c_str());
      return false;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("[OTA] No update.");
      return false;
    case HTTP_UPDATE_OK:
      Serial.println("[OTA] Success. Rebooting...");
      return true;
  }
  return false;
}

// ─────────────────────────────────────────────────────────────────────────────
void NetManager::loop() {
  // Auto-reconnect
  if (!WiFi.isConnected() && _connected) {
    Serial.println("[NET] WiFi dropped. Reconnecting...");
    _reconnect();
  }

  // Periodic OTA check
  if (_connected) {
    uint32_t now = millis();
    if (_lastOTACheck == 0 || (now - _lastOTACheck) > OTA_CHECK_INTERVAL) {
      _lastOTACheck = now;
      checkOTA();
    }
  }
}

void NetManager::_reconnect() {
  WiFi.reconnect();
  uint32_t start = millis();
  while (!WiFi.isConnected() && millis() - start < 15000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  _connected = WiFi.isConnected();
  if (_connected) Serial.println("[NET] Reconnected.");
  else            Serial.println("[NET] Reconnect failed.");
}

bool   NetManager::isConnected() const { return WiFi.isConnected(); }
String NetManager::getIP()       const { return WiFi.localIP().toString(); }
String NetManager::getSSID()     const { return WiFi.SSID(); }
