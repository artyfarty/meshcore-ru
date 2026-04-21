# MeshCore Firmware — Moscow Mesh Fork

Форк прошивки [MeshCore](https://github.com/ripplebiz/MeshCore) для companion radio с патчами для московской mesh-сети.

Долгосрочная поддержка не гарантируется. Используйте на свой страх и риск.

## Изменения

### Кириллица на OLED/TFT
Кастомный 5x7 шрифт с глифами А-Яа-яЁё (0x80-0xC1). Pre-build скрипт (`patch_font.py`) патчит `glcdfont.c` в Adafruit GFX при каждой сборке.

UI (`companion_radio/ui-new`) прогоняет все строки через `translateUTF8ToBlocks` до отрисовки. Переопределяем его в `SSD1306Display` и `ST7789LCDDisplay`: кириллица UTF-8 маппится в индексы 0x80-0xC1, что корректно работает и с `getTextWidth`/`drawTextEllipsized` (единственный байт на символ). `SSD1306Display::print` также декодирует сырой UTF-8 — для code paths, минующих UI-слой.

### Фильтр Public канала на экране
Сообщения из Public канала (index 0) не показываются на OLED. Канал по-прежнему работает и виден в приложении.

### Power Off (true shutdown)
- **Heltec V3/V4**: отключает VEXT (периферия), deep sleep без wake sources. Потребление ~µA. Пробуждение только по RST.
- **T-Beam**: вызывает `PMU->shutdown()` через AXP PMU. Полное обесточивание. Пробуждение только по RST.
- Пункт меню переименован из "hibernate" в "power off".

### WiFi Companion для T-Beam SX1276
Добавлен build target `Tbeam_SX1276_companion_radio_wifi` — companion radio с WiFi TCP:5000 вместо BLE.

## Поддерживаемые устройства

Собрано и протестировано:
- Heltec V3 (BLE companion)
- Heltec V4 OLED (BLE companion)
- LilyGo T-Beam v1.1/v1.2 SX1276 (BLE companion)
- LilyGo T-Beam v1.1/v1.2 SX1276 (WiFi companion)

## Сборка

```bash
# Heltec V3 BLE
pio run -e Heltec_v3_companion_radio_ble

# Heltec V4 BLE (OLED)
pio run -e heltec_v4_companion_radio_ble

# T-Beam SX1276 BLE
pio run -e Tbeam_SX1276_companion_radio_ble

# T-Beam SX1276 WiFi
pio run -e Tbeam_SX1276_companion_radio_wifi
```

## Прошивка

```bash
# Полная (с очисткой)
esptool.py --chip <esp32|esp32s3> --port /dev/ttyXXX --baud 921600 erase_flash
esptool.py --chip <esp32|esp32s3> --port /dev/ttyXXX --baud 921600 \
  write_flash 0x0 bootloader.bin 0x8000 partitions.bin 0x10000 firmware.bin

# Обновление (без очистки, сохраняет ключи и контакты)
esptool.py --chip <esp32|esp32s3> --port /dev/ttyXXX --baud 921600 \
  write_flash 0x10000 firmware.bin
```

---

Форк создан при помощи [Claude Code](https://claude.ai/claude-code) (Anthropic).

Upstream: https://github.com/ripplebiz/MeshCore
