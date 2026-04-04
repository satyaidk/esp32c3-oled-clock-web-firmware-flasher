#pragma once
#include <Adafruit_SSD1306.h>
#include "config.h"

// ── Clock faces ─────────────────────────────
enum ClockFace {
  FACE_DIGITAL = 0,   // large bold digital time
  FACE_ANALOG  = 1,   // classic analog clock
  FACE_MINIMAL = 2,   // tiny time + big date
  FACE_COUNT   = 3
};

class DisplayManager {
public:
  DisplayManager();
  bool     begin();
  void     update(struct tm *t);

  void     showBootScreen();
  void     showWiFiSetup(const char *apName, const char *apPass);
  void     showConnecting(const char *ssid);
  void     showConnected(const char *ip);
  void     showOTAProgress(int pct);
  void     showError(const char *msg);
  void     showMessage(const char *line1, const char *line2 = nullptr);

  void     nextFace();
  void     setBrightness(uint8_t level);   // 0-255
  void     setDimmed(bool dim);
  ClockFace getFace() const { return _face; }

private:
  Adafruit_SSD1306 _oled;
  ClockFace        _face      = FACE_DIGITAL;
  bool             _dimmed    = false;
  uint8_t          _brightness = 200;

  // Face renderers
  void _drawDigital(struct tm *t);
  void _drawAnalog(struct tm *t);
  void _drawMinimal(struct tm *t);

  // Shared helpers
  void _drawStatusBar(struct tm *t);
  void _drawAnalogHand(int cx, int cy, float angle, int len, uint8_t thick, uint16_t col);
  void _centerStr(const char *s, int y, uint8_t size = 1);
  void _drawBattery(int x, int y);          // placeholder icon
};
