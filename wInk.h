#ifndef _WINK_H_
#define _WINK_H_

#include<SPI.h>
#include<Adafruit_GFX.h>

#if defined (ESP8266) || defined (__STM32F1__) || defined (ESP32)
typedef volatile uint32_t PortReg;
typedef uint32_t PortMask;
#endif

// EPD2IN9 commands
#define WINK_DRIVER_OUTPUT_CONTROL			0x01
#define WINK_BOOSTER_SOFT_START_CONTROL		0x0C
#define WINK_GATE_SCAN_START_POSITION		0x0F
#define WINK_DEEP_SLEEP_MODE				0x10
#define WINK_DATA_ENTRY_MODE_SETTING			0x11
#define WINK_SW_RESET				0x12
#define WINK_TEMPERATURE_SENSOR_CONTROL		0x1A
#define WINK_MASTER_ACTIVATION			0x20
#define WINK_DISPLAY_UPDATE_CONTROL_1		0x21
#define WINK_DISPLAY_UPDATE_CONTROL_2		0x22
#define WINK_WRITE_RAM				0x24
#define WINK_WRITE_VCOM_REGISTER			0x2C
#define WINK_WRITE_LUT_REGISTER			0x32
#define WINK_SET_DUMMY_LINE_PERIOD			0x3A
#define WINK_SET_GATE_TIME				0x3B
#define WINK_BORDER_WAVEFORM_CONTROL			0x3C
#define WINK_SET_RAM_X_ADDRESS_START_END_POSITION	0x44
#define WINK_SET_RAM_Y_ADDRESS_START_END_POSITION	0x45
#define WINK_SET_RAM_X_ADDRESS_COUNTER		0x4E
#define WINK_SET_RAM_Y_ADDRESS_COUNTER		0x4F
#define WINK_TERMINATE_FRAME_READ_WRITE		0xFF

#define WINK_BLACK 0
#define WINK_WHITE 1
#define WINK_INVERSE 2

#define WAVESHARE_2DOT9	128, 296, false
//2DOT13 visible resolution is 122x250
#define WAVESHARE_2DOT13 128, 250, false
#define WAVESHARE_1DOT54 200, 200, false

#define WAVESHARE_2DOT9_TRICOLOUR	128, 296, true

class wInkDisplay : public Adafruit_GFX {
 public:
  wInkDisplay(int16_t wWidth, int16_t wHeight, bool tricolour, int8_t BUSY, int8_t RST, int8_t DC, int8_t CS = SS, SPIClass *useSPI = &SPI);
  ~wInkDisplay();
  void drawPixel(int16_t x, int16_t y, uint16_t colour);
  void sendCommand(uint8_t comm);
  void sendData(uint8_t data);
  /*
  This tells the display to only do a partial update.
  Faster, doesn't go full black-white cycle,
  but may retain a ghost-image of prev. content.
  */
  void setLutPartial();
  /*
  This tells the display to do a full update,
  ie. cycles to black and white,
  more likely no ghost-image left behind.
  */
  void setLutFull();
  bool begin(bool useTiledMemory = false);
  void busyWait();
  bool isBusy();
  void clearDisplay(uint8_t colour);
  void display(bool waitUntilFinished = true);
  /*
  The higher the contrast, the stronger any ghosting
  on the display will be.
  */
  void setContrast(uint8_t _contrast);
  void wakeUp();
  void sleep();
  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t colour);
  void drawFastHLineAbsolute(int16_t x, int16_t y, int16_t w, uint16_t colour);
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t colour);
  void drawFastVLineAbsolute(int16_t x, int16_t y, int16_t h, uint16_t colour);

 private:
  int8_t busy, rst, dc, cs;
  SPIClass *spi = NULL;
  uint8_t *buffer = NULL;
  uint8_t contrast = 0xA8;
  const uint8_t * lut = NULL;
  /*volatile PortReg *dcPort, *csPort, *busyPort;
  PortMask csPinmask, dcPinmask, busyPinmask;*/
  SPISettings spiSettings;

  void _sendCommand(uint8_t comm);
  void _sendData(uint8_t data);
  void _setLut(const uint8_t * _lut);
  void setMemoryArea(int16_t x_start, int16_t y_start, int16_t x_end, int16_t y_end);
  void setMemoryPointer(int16_t x, int16_t y);
  void drawAbsolutePixel(int16_t x, int16_t y, uint16_t colour);
};
#endif