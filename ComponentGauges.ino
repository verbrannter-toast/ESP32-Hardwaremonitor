#include <ArduinoJson.h>
#include <TFT_eSPI.h>

#define ARDUINO_USB_MODE 1
#define ARDUINO_USB_CDC_ON_BOOT 1

TFT_eSPI tft = TFT_eSPI();

// Farben
uint16_t to565(uint8_t r, uint8_t g, uint8_t b) {
  return ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3);
}
uint16_t COL_BG, COL_RING_BG, COL_TEXT, COL_GREEN, COL_YELLOW, COL_RED;

uint16_t getColorForValue(float val) {
  if (val < 50) return COL_GREEN;
  if (val < 75) return COL_YELLOW;
  return COL_RED;
}

// Arc-Zeichnung
void drawArc(int cx, int cy, int r, int thickness, float startAngle, float endAngle, uint16_t color) {
  for (float angle = startAngle; angle <= endAngle; angle += 0.5) {
    float rad = angle * 0.0174533;
    for (int w = 0; w < thickness; w++) {
      int x = cx + cos(rad) * (r - w);
      int y = cy + sin(rad) * (r - w);
      tft.drawPixel(x, y, color);
    }
  }
}

// Gauge zeichnen
void drawCircularGauge(int x, int y, int r, int thickness, float value, const char* label) {
  float startAngle = -135;
  float endAngle = 135;
  float maxAngle = startAngle + ((endAngle - startAngle) * (value / 100.0f));
  uint16_t col = getColorForValue(value);

  tft.fillCircle(x, y, r + 2, COL_BG);
  drawArc(x, y, r, thickness, startAngle, endAngle, COL_RING_BG);
  drawArc(x, y, r, thickness, startAngle, maxAngle, col);

  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(COL_TEXT, COL_BG);
  tft.drawString(label, x, y - 10, 2);

  String valText;
  if (value < 1.0)
    valText = String(value, 1) + "%";
  else
    valText = String((int)value) + "%";

  tft.drawString(valText, x, y + 10, 2);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Booted");
  tft.init();
  tft.setRotation(1);

  COL_BG      = to565(0, 0, 0);
  COL_RING_BG = to565(40, 70, 90);
  COL_TEXT    = to565(255, 255, 255);
  COL_GREEN   = to565(0, 220, 80);
  COL_YELLOW  = to565(255, 210, 0);
  COL_RED     = to565(240, 60, 40);

  tft.fillScreen(COL_BG);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(COL_TEXT);
  tft.drawString("...", 160, 120, 2);
}

void loop() {
  static String buffer;

  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      StaticJsonDocument<256> doc;
      DeserializationError err = deserializeJson(doc, buffer);

      if (!err) {
        float cpu  = doc["cpu"];
        float ram  = doc["ram"];
        float disk = doc["disk"];
        float temp = doc["temp"];
        float watt = doc["watt"];

        drawCircularGauge( 50, 120, 50, 6, cpu,  "CPU");
        drawCircularGauge(160, 120, 50, 6, ram,  "RAM");
        drawCircularGauge(270, 120, 50, 6, disk, "Disk");

        // Temperatur unten links
        tft.setTextDatum(BL_DATUM);
        tft.setTextColor(COL_TEXT, COL_BG);
        if (!isnan(temp))
          tft.drawString(String(temp, 1) + " °C", 5, 235, 2);

        // Watt unten rechts
        tft.setTextDatum(BR_DATUM);
        tft.setTextColor(COL_TEXT, COL_BG);
        if (!isnan(watt))
          tft.drawString("   " + String(watt, 1) + " W", 315, 235, 2);
      }

      buffer = "";
    } else {
      buffer += c;
    }
  }
}
