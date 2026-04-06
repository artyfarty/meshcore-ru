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
  // UTF-8 decode with Cyrillic mapping to custom font indices 0x80-0xC1
  // А-Я = 0x80-0x9F, а-я = 0xA0-0xBF, Ё = 0xC0, ё = 0xC1
  const uint8_t* p = (const uint8_t*)str;
  while (*p) {
    if (*p < 0x80) {
      display.write(*p++);
    } else if (p[0] == 0xD0 && p[1] >= 0x90 && p[1] <= 0xAF) {
      // А-Я (U+0410-U+042F) → 0x80-0x9F
      display.write(0x80 + (p[1] - 0x90));
      p += 2;
    } else if (p[0] == 0xD0 && p[1] >= 0xB0 && p[1] <= 0xBF) {
      // а-п (U+0430-U+043F) → 0xA0-0xAF
      display.write(0xA0 + (p[1] - 0xB0));
      p += 2;
    } else if (p[0] == 0xD1 && p[1] >= 0x80 && p[1] <= 0x8F) {
      // р-я (U+0440-U+044F) → 0xB0-0xBF
      display.write(0xB0 + (p[1] - 0x80));
      p += 2;
    } else if (p[0] == 0xD0 && p[1] == 0x81) {
      // Ё (U+0401) → 0xC0
      display.write(0xC0);
      p += 2;
    } else if (p[0] == 0xD1 && p[1] == 0x91) {
      // ё (U+0451) → 0xC1
      display.write(0xC1);
      p += 2;
    } else if (*p >= 0xC0) {
      // Skip unknown multi-byte UTF-8
      p++;
      while (*p && (*p & 0xC0) == 0x80) p++;
    } else {
      display.write('?');
      p++;
    }
  }
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
