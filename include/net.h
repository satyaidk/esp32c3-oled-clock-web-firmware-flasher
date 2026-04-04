#pragma once
#include <Arduino.h>

class NetManager {
public:
  NetManager();

  // Connect via WiFiManager captive portal. Returns true if connected.
  bool     connect();

  // Sync time over NTP. Returns true on success.
  bool     syncNTP();

  // Check OTA server for newer firmware. Returns true if update applied.
  bool     checkOTA();

  // Getters
  bool     isConnected() const;
  String   getIP() const;
  String   getSSID() const;

  // Must be called periodically (handles WiFi reconnect + OTA timer)
  void     loop();

private:
  bool     _connected    = false;
  bool     _ntpSynced    = false;
  uint32_t _lastOTACheck = 0;

  void     _reconnect();
  bool     _fetchAndUpdate(const String &url);
};
