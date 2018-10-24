#include<wInk.h>
#include"avatar.h"

// 128x296 display, BUSY-pin 27, RST-pin 16, DC-pin 17 on ESP32
wInkDisplay display(WAVESHARE_2DOT9, 27, 16, 17);

void setup() {
  Serial.begin(115200);

  if(!display.begin()){
    Serial.println("display.begin() failed, couldn't allocate memory for the display-buffer!");
    while(true) delay(1000);
  }

  Serial.println("Draw something on the display...");
  display.setTextColor(WINK_BLACK);
  display.clearDisplay(WINK_WHITE);
  display.setRotation(3);
  display.setCursor(2, 2);
  display.println("e-Ink Test!");
#if defined (ESP8266) || defined (ESP32)
  display.setCursor(2, display.getCursorY());
  display.println("Running on ESP32");
  display.setCursor(2, display.getCursorY());
  display.println("Free heap: " + String(ESP.getFreeHeap()));
#endif
  display.drawRect(0, 0, display.width(), display.height(), WINK_BLACK);
  display.drawFastHLine(10, 72, 27, WINK_BLACK);
  display.drawPixel(10, 74, WINK_BLACK);
  display.drawPixel(36, 74, WINK_BLACK);
  display.drawFastVLine(10, 46, 27, WINK_BLACK);
  display.drawPixel(8, 46, WINK_BLACK);
  display.drawPixel(8, 72, WINK_BLACK);
  display.drawBitmap(display.width() - 128 - 1, display.height() - 128 - 1, avatar, 128, 128, WINK_BLACK);

  display.display();
  //You should put these displays to sleep when not actively needed
  display.sleep();
}

void loop() {
  delay(10000);
}
