#include<wInk.h>
#include"avatar.h"

// 128x296 display, BUSY-pin 27, RST-pin 16, DC-pin 17 on ESP32
wInkDisplay display(WAVESHARE_2DOT9_TRICOLOUR, 27, 16, 17);

uint8_t rot = 0;

void setup() {
  Serial.begin(115200);

  if(!display.begin()){
    Serial.println("display.begin() failed, couldn't allocate memory for the display-buffer!");
    while(true) delay(1000);
  }
  Serial.println("Clear display to white...");
  display.clearDisplay(WINK_WHITE);
  display.display();
  Serial.println("Sleep 15s.");
  delay(15000);
  Serial.println("Clear display to black...");
  display.clearDisplay(WINK_BLACK);
  display.display();
  Serial.println("Sleep 15s.");
  delay(15000);
  Serial.println("Clear display to colour...");
  display.clearDisplay(WINK_COLOUR);
  display.display();
  Serial.println("Sleep 15s.");
  display.sleep();
  delay(15000);
}

void loop() {
  Serial.println("Draw something on the display...");
  display.clearDisplay(WINK_WHITE);
  display.setRotation(rot);
  display.setCursor(2, 2);
  display.setTextColor(WINK_COLOUR);
  display.println("e-Ink Test!");
  display.setTextColor(WINK_BLACK);
  display.setCursor(2, display.getCursorY());
  display.println("Should be working now.");
#if defined (ESP8266) || defined (ESP32)
  display.setCursor(2, display.getCursorY());
  display.println("Running on ESP32");
  display.setCursor(2, display.getCursorY());
  display.println("Free heap: " + String(ESP.getFreeHeap()));
#endif
  display.drawRect(0, 0, display.width(), display.height(), WINK_BLACK);
  display.drawFastHLine(10, 72, 27, WINK_COLOUR);
  display.drawPixel(10, 74, WINK_COLOUR);
  display.drawPixel(36, 74, WINK_COLOUR);
  display.drawFastVLine(10, 46, 27, WINK_BLACK);
  display.drawPixel(8, 46, WINK_COLOUR);
  display.drawPixel(8, 72, WINK_COLOUR);

  display.drawFastHLine(60, 72, 27, WINK_BLACK);
  display.drawPixel(60, 74, WINK_BLACK);
  display.drawPixel(86, 74, WINK_BLACK);
  display.drawFastVLine(60, 46, 27, WINK_COLOUR);
  display.drawPixel(58, 46, WINK_BLACK);
  display.drawPixel(58, 72, WINK_BLACK);

  display.fillRect(0, 0, 3, 3, WINK_COLOUR);
  display.fillRect(display.width() - 4, 0, 3, 3, WINK_COLOUR);
  display.fillRect(display.width() - 4, display.height() - 4, 3, 3, WINK_COLOUR);
  display.fillRect(0, display.height() - 4, 3, 3, WINK_COLOUR);

  display.drawBitmap(display.width() - 128 - 1, display.height() - 128 - 1, avatarBlack, 128, 128, WINK_BLACK);
  display.drawBitmap(display.width() - 128 - 1, display.height() - 128 - 1, avatarRed, 128, 128, WINK_COLOUR);

  display.display();
  //You should put these displays to sleep when not actively needed
  display.sleep();
  delay(10000);
  display.wakeUp();
  if(rot++ > 3) rot = 0;
}