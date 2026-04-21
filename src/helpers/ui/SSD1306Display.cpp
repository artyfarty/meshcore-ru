#include "SSD1306Display.h"
#include "glcdfont_ru.h"

bool SSD1306Display::i2c_probe(TwoWire& wire, uint8_t addr) {
  wire.beginTransmission(addr);
  uint8_t error = wire.endTransmission();
  return (error == 0);
}

bool SSD1306Display::begin() {
  if (!_isOn) {
    if (_peripher_power) _peripher_power->claim();
    _isOn = true;
  }
  #ifdef DISPLAY_ROTATION
  display.setRotation(DISPLAY_ROTATION);
  #endif
  return display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS, true, false) && i2c_probe(Wire, DISPLAY_ADDRESS);
}

void SSD1306Display::turnOn() {
  if (!_isOn) {
    if (_peripher_power) _peripher_power->claim();
    _isOn = true;  // set before begin() to prevent double claim
    if (_peripher_power) begin();  // re-init display after power was cut
  }
  display.ssd1306_command(SSD1306_DISPLAYON);
}

void SSD1306Display::turnOff() {
  display.ssd1306_command(SSD1306_DISPLAYOFF);
  if (_isOn) {
    if (_peripher_power) {
#if PIN_OLED_RESET >= 0
      digitalWrite(PIN_OLED_RESET, LOW);
#endif
      _peripher_power->release();
    }
    _isOn = false;
  }
}

void SSD1306Display::clear() {
  display.clearDisplay();
  display.display();
}

void SSD1306Display::startFrame(Color bkg) {
  display.clearDisplay();  // TODO: apply 'bkg'
  _color = SSD1306_WHITE;
  display.setTextColor(_color);
  display.setTextSize(1);
  display.cp437(true);
}

void SSD1306Display::setTextSize(int sz) {
  display.setTextSize(sz);
}

void SSD1306Display::setColor(Color c) {
  _color = (c != 0) ? SSD1306_WHITE : SSD1306_BLACK;
  display.setTextColor(_color);
}

void SSD1306Display::setCursor(int x, int y) {
  display.setCursor(x, y);
}

void SSD1306Display::print(const char* str) {
  // Accept either raw UTF-8 or already-remapped Cyrillic bytes (0x80-0xC1).
  // UI typically calls translateUTF8ToBlocks() first (below), so most inputs
  // are already single-byte. This decoder still handles raw UTF-8 for code
  // paths that bypass the UI layer.
  const uint8_t* p = (const uint8_t*)str;
  while (*p) {
    if (p[0] == 0xD0 && p[1] >= 0x90 && p[1] <= 0xAF) {
      display.write(0x80 + (p[1] - 0x90)); p += 2;
    } else if (p[0] == 0xD0 && p[1] >= 0xB0 && p[1] <= 0xBF) {
      display.write(0xA0 + (p[1] - 0xB0)); p += 2;
    } else if (p[0] == 0xD1 && p[1] >= 0x80 && p[1] <= 0x8F) {
      display.write(0xB0 + (p[1] - 0x80)); p += 2;
    } else if (p[0] == 0xD0 && p[1] == 0x81) {
      display.write(0xC0); p += 2;
    } else if (p[0] == 0xD1 && p[1] == 0x91) {
      display.write(0xC1); p += 2;
    } else {
      display.write(*p++);  // ASCII or pre-remapped byte — write as-is
    }
  }
}

void SSD1306Display::translateUTF8ToBlocks(char* dest, const char* src, size_t dest_size) {
  utf8_to_cyrillic_cp437(dest, src, dest_size);
}

void SSD1306Display::fillRect(int x, int y, int w, int h) {
  display.fillRect(x, y, w, h, _color);
}

void SSD1306Display::drawRect(int x, int y, int w, int h) {
  display.drawRect(x, y, w, h, _color);
}

void SSD1306Display::drawXbm(int x, int y, const uint8_t* bits, int w, int h) {
  display.drawBitmap(x, y, bits, w, h, SSD1306_WHITE);
}

uint16_t SSD1306Display::getTextWidth(const char* str) {
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(str, 0, 0, &x1, &y1, &w, &h);
  return w;
}

void SSD1306Display::endFrame() {
  display.display();
}
