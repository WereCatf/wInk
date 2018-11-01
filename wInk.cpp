#include<wInk.h>
#define paramSwapint16_t(a, b) { int16_t t = a; a = b; b = t; }

const uint8_t lut_full_update_2DOT9[] __attribute__((aligned(4))) PROGMEM =
{
	0x02, 0x02, 0x01, 0x11, 0x12, 0x12, 0x22, 0x22, 
	0x66, 0x69, 0x69, 0x59, 0x58, 0x99, 0x99, 0x88, 
	0x00, 0x00, 0x00, 0x00, 0xF8, 0xB4, 0x13, 0x51, 
	0x35, 0x51, 0x51, 0x19, 0x01, 0x00
};

const uint8_t lut_partial_update_2DOT9[] __attribute__((aligned(4))) PROGMEM =
{
	0x10, 0x18, 0x18, 0x08, 0x18, 0x18, 0x08, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x13, 0x14, 0x44, 0x12, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t lut_full_update_2DOT13[] __attribute__((aligned(4))) PROGMEM =
{
    0x22, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x11, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t lut_partial_update_2DOT13[] __attribute__((aligned(4))) PROGMEM =
{
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x0F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t lut_full_update_1DOT54[] __attribute__((aligned(4))) PROGMEM =
{
    0x02, 0x02, 0x01, 0x11, 0x12, 0x12, 0x22, 0x22, 
    0x66, 0x69, 0x69, 0x59, 0x58, 0x99, 0x99, 0x88, 
    0x00, 0x00, 0x00, 0x00, 0xF8, 0xB4, 0x13, 0x51, 
    0x35, 0x51, 0x51, 0x19, 0x01, 0x00
};

const uint8_t lut_partial_update_1DOT54[] __attribute__((aligned(4))) PROGMEM =
{
    0x10, 0x18, 0x18, 0x08, 0x18, 0x18, 0x08, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x13, 0x14, 0x44, 0x12, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

wInkDisplay::wInkDisplay(int16_t wWidth, int16_t wHeight, bool _tricolour, int8_t BUSY, int8_t RST, int8_t DC, int8_t CS, SPIClass *useSPI) : Adafruit_GFX(wWidth, wHeight) {
	busy = BUSY;
	rst = RST;
	dc = DC;
	cs = CS;
	spi = useSPI;
	buffer = NULL;
	colourBuffer = NULL;
	tricolour = _tricolour;
	lut = lut_full_update_2DOT9;
	PHYSWIDTH = wWidth;
	PHYSHEIGHT = wHeight;
	if(wWidth == 122 && wHeight == 250){
		lut = lut_full_update_2DOT13;
		PHYSWIDTH = 128;
	}
	else if(wWidth == 200 && wHeight == 200) lut = lut_full_update_1DOT54;
	spiSettings = SPISettings(2000000, MSBFIRST, SPI_MODE0);
}

wInkDisplay::~wInkDisplay(){
	free(buffer);
	free(colourBuffer);
}

bool wInkDisplay::begin(bool useTiledMemory) {
	if(PHYSWIDTH == 0 || PHYSHEIGHT == 0) return false;

	//If we're waking from sleep, buffer won't be NULL
	//and none of this stuff is needed, since it's
	//already been done once
	if(buffer == NULL){
		buffer = reinterpret_cast<uint8_t*>(malloc(PHYSWIDTH * PHYSHEIGHT / 8));
		if (buffer == NULL) return false;
		memset(buffer, 0, (PHYSWIDTH*PHYSHEIGHT/8));

		pinMode(cs, OUTPUT);
		pinMode(rst, OUTPUT);
		pinMode(dc, OUTPUT);
		pinMode(busy, INPUT);
		/*_csport = portOutputRegister(digitalPinToPort(_cs));
		_cspinmask = digitalPinToBitMask(_cs);
		_dcport = portOutputRegister(digitalPinToPort(_dc));
		_dcpinmask = digitalPinToBitMask(_dc);
		_busyport = portOutputRegister(digitalPinToPort(_busy));
		_busypinmask = digitalPinToBitMask(_busy);*/

		spi->begin();
	}
	if(colourBuffer == NULL && tricolour){
		colourBuffer = reinterpret_cast<uint8_t*>(malloc(PHYSWIDTH * PHYSHEIGHT / 8));
		if (colourBuffer == NULL) return false;
		memset(colourBuffer, 0, (PHYSWIDTH*PHYSHEIGHT/8));
	}

	//Reset display, wake the display from sleep
	digitalWrite(rst, LOW);
	delay(200);
	digitalWrite(rst, HIGH);
	delay(200);

	spi->beginTransaction(spiSettings);

	if(!tricolour){
		_sendCommand(WINK_DRIVER_OUTPUT_CONTROL);
		_sendData((PHYSHEIGHT - 1) & 0xFF);
		_sendData(((PHYSHEIGHT - 1) >> 8) & 0xFF);
		_sendData(0x00);	// GD = 0; SM = 0; TB = 0;
		_sendCommand(WINK_BOOSTER_SOFT_START_CONTROL);
		_sendData(0xD7);
		_sendData(0xD6);
		_sendData(0x9D);
		_sendCommand(WINK_WRITE_VCOM_REGISTER);
		_sendData(contrast);
		_sendCommand(WINK_SET_DUMMY_LINE_PERIOD);
		_sendData(0x1A);	// 4 dummy lines per gate
		_sendCommand(WINK_SET_GATE_TIME);
		_sendData(0x08);	// 2us per line
		_sendCommand(WINK_DATA_ENTRY_MODE_SETTING);
		_sendData(0x03);	// X increment; Y increment
		_setLut(lut);
	} else {
		Serial.println("Debug1");
		_sendCommand(WINK_TRI_BOOSTER_SOFT_START);
		_sendData(0x17);
		_sendData(0x17);
		_sendData(0x17);
		_sendCommand(WINK_TRI_POWER_ON);
		busyWait();
		_sendCommand(WINK_TRI_PANEL_SETTING);
		_sendData(0x8F);
		_sendCommand(WINK_TRI_VCOM_AND_DATA_INTERVAL_SETTING);
		_sendData(0x77);
		_sendCommand(WINK_TRI_TCON_RESOLUTION);
		_sendData(0x80);
		_sendData(0x01);
		_sendData(0x28);
		_sendCommand(WINK_TRI_VCM_DC_SETTING_REGISTER);
		_sendData(0X0A);
	}

	spi->endTransaction();
	return true;
}

inline void wInkDisplay::drawAbsolutePixel(int16_t x, int16_t y, uint16_t colour) {
	if(colour > 3) Serial.println("Colour > 3: " + String(colour));
	if(x < 0 || x >= PHYSWIDTH || y < 0 || y >= PHYSHEIGHT) return;
	if(colour == WINK_INVERSE){
		buffer[(x + y * PHYSWIDTH) / 8] ^= 0x80 >> (x % 8);
		if(tricolour) colourBuffer[(x + y * PHYSWIDTH) / 8] |= 0x80 >> (x % 8);
		return;
	}
	switch(colour){
		case WINK_BLACK:
			buffer[(x + y * PHYSWIDTH) / 8] &= ~(0x80 >> (x % 8));
			if(tricolour) colourBuffer[(x + y * PHYSWIDTH) / 8] |= 0x80 >> (x % 8);
			break;
		case WINK_WHITE:
			buffer[(x + y * PHYSWIDTH) / 8] |= 0x80 >> (x % 8);
			if(tricolour) colourBuffer[(x + y * PHYSWIDTH) / 8] |= 0x80 >> (x % 8);
			break;
		case WINK_COLOUR:
			if(!tricolour) break;
			colourBuffer[(x + y * PHYSWIDTH) / 8] &= ~(0x80 >> (x % 8));
			buffer[(x + y * PHYSWIDTH) / 8] |= 0x80 >> (x % 8);
			break;
	}
}

void wInkDisplay::drawPixel(int16_t x, int16_t y, uint16_t colour) {
	if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
		return;

	switch (getRotation()) {
		case 0:
			drawAbsolutePixel(x, y, colour);
			break;
		case 1:
			drawAbsolutePixel(PHYSWIDTH - y - 1, x, colour);
			break;
		case 2:
			drawAbsolutePixel(PHYSWIDTH - x - 1, PHYSHEIGHT - y - 1, colour);
			break;
		case 3:
			drawAbsolutePixel(y, PHYSHEIGHT - x - 1, colour);
			break;
	}
}

void wInkDisplay::sendCommand(uint8_t comm){
	spi->beginTransaction(spiSettings);
	_sendCommand(comm);
	spi->endTransaction();
}

void wInkDisplay::_sendCommand(uint8_t comm){
	//Write pin LOW
	/**_dcport &= ~_dcpinmask;
	*_csport &= ~_cspinmask;*/
	digitalWrite(dc, LOW);
	digitalWrite(cs, LOW);
	spi->transfer(comm);
	//Write pin HIGH
	//*_csport |= _cspinmask;
	digitalWrite(cs, HIGH);
}

void wInkDisplay::sendData(uint8_t data){
	spi->beginTransaction(spiSettings);
	_sendData(data);
	spi->endTransaction();
}

void wInkDisplay::_sendData(uint8_t data){
	//Write pin HIGH
	//*_dcport |= _dcpinmask;
	digitalWrite(dc, HIGH);
	//Write pin LOW
	digitalWrite(cs, LOW);
	//*_csport &= ~_cspinmask;

	spi->transfer(data);
	//Write pin HIGH
	digitalWrite(cs, HIGH);
	//*_csport |= _cspinmask;
}

void wInkDisplay::setLutFull() {
	if(tricolour) return;
	spi->beginTransaction(spiSettings);
	lut = lut_full_update_2DOT9;
	if(PHYSWIDTH == 128 && PHYSHEIGHT == 250) lut = lut_full_update_2DOT13;
	else if(PHYSWIDTH == 200 && PHYSHEIGHT == 200) lut = lut_full_update_1DOT54;
	_setLut(lut);
	spi->endTransaction();
}

void wInkDisplay::setLutPartial() {
	if(tricolour) return;
	spi->beginTransaction(spiSettings);
	lut = lut_partial_update_2DOT9;
	if(PHYSWIDTH == 128 && PHYSHEIGHT == 250) lut = lut_partial_update_2DOT13;
	else if(PHYSWIDTH == 200 && PHYSHEIGHT == 200) lut = lut_full_update_1DOT54;
	_setLut(lut);
	spi->endTransaction();
}

void wInkDisplay::_setLut(const uint8_t * _lut) {
	if(tricolour) return;
	busyWait();
	lut = _lut;
	_sendCommand(WINK_WRITE_LUT_REGISTER);

	//Write pin HIGH
	//*_dcport |= _dcpinmask;
	digitalWrite(dc, HIGH);
	//Write pin LOW
	digitalWrite(cs, LOW);
	//*_csport &= ~_cspinmask;
	for(uint8_t i=0; i < 30; i++)
		spi->transfer(pgm_read_byte(_lut + i));
	//Write pin HIGH
	digitalWrite(cs, HIGH);
	//*_csport |= _cspinmask;
}

void wInkDisplay::setContrast(uint8_t _contrast) {
	if(tricolour) return;
	//The higher the value we send, the lower the actual contrast, so invert
	contrast = ~_contrast;
	spi->beginTransaction(spiSettings);
	_sendCommand(WINK_WRITE_VCOM_REGISTER);
	_sendData(_contrast);
	spi->endTransaction();
}

void wInkDisplay::busyWait() {
	if(tricolour)
		while(!digitalRead(busy)) delay(1);
	else
		while(digitalRead(busy)) delay(1);
}

bool wInkDisplay::isBusy() {
	if(tricolour) return !digitalRead(busy);
	return digitalRead(busy);
}

void wInkDisplay::clearDisplay(uint16_t colour) {
	if(buffer == NULL) return;
	if(tricolour && colourBuffer == NULL) return;

	if(colour == WINK_INVERSE){
		for(uint32_t i=0; i < (PHYSWIDTH*PHYSHEIGHT/8); i++)
			*(buffer + i) = ~*(buffer + i);
		return;
	}

	if(colour == WINK_WHITE) colour = 0xFF;
	else if(colour == WINK_BLACK) colour = 0x00;
	if(tricolour){
		if(colour == WINK_COLOUR){
			memset(colourBuffer, 0x00, (PHYSWIDTH*PHYSHEIGHT/8));
			colour = 0xFF;
		} else
			memset(colourBuffer, 0xFF, (PHYSWIDTH*PHYSHEIGHT/8));
	}
	memset(buffer, colour, (PHYSWIDTH*PHYSHEIGHT/8));
}

void wInkDisplay::display(bool waitUntilFinished) {
	if(tricolour){
		if(buffer == NULL || colourBuffer == NULL) return;
		busyWait();
		spi->beginTransaction(spiSettings);

		_sendCommand(WINK_TRI_TCON_RESOLUTION);
		_sendData(PHYSWIDTH >> 8);
		_sendData(PHYSWIDTH & 0xff);
		_sendData(PHYSHEIGHT >> 8);        
		_sendData(PHYSHEIGHT & 0xff);

		_sendCommand(WINK_TRI_DATA_START_TRANSMISSION_1);           
		delay(2);
		digitalWrite(dc, HIGH);
		digitalWrite(cs, LOW);

#if defined (ESP8266) || defined (ESP32)
		spi->writeBytes((uint8_t *)buffer, PHYSWIDTH * PHYSHEIGHT / 8);
#elif defined (__STM32F1__)
		spi->write((const void *)buffer, PHYSWIDTH * PHYSHEIGHT / 8);
#else
		for (uint16_t i = 0; i < (PHYSWIDTH * PHYSHEIGHT / 8); i++)
			spi->transfer(buffer[i]);
#endif
		digitalWrite(cs, HIGH);
		delay(2);
		_sendCommand(WINK_TRI_DATA_START_TRANSMISSION_2);           
		delay(2);
		digitalWrite(dc, HIGH);
		digitalWrite(cs, LOW);

#if defined (ESP8266) || defined (ESP32)
		spi->writeBytes((uint8_t *)colourBuffer, PHYSWIDTH * PHYSHEIGHT / 8);
#elif defined (__STM32F1__)
		spi->write((const void *)colourBuffer, PHYSWIDTH * PHYSHEIGHT / 8);
#else
		for (uint16_t i = 0; i < (PHYSWIDTH * PHYSHEIGHT / 8); i++)
			spi->transfer(colourBuffer[i]);
#endif
		digitalWrite(cs, HIGH);
		delay(2);

		_sendCommand(WINK_TRI_DISPLAY_REFRESH); 
		spi->endTransaction();
		if(waitUntilFinished) busyWait();
	} else {
		if(buffer == NULL) return;
		busyWait();
		spi->beginTransaction(spiSettings);

		_setMemoryArea(0, 0, PHYSWIDTH - 1, PHYSHEIGHT - 1);
		_setMemoryPointer(0, 0);
		_sendCommand(WINK_WRITE_RAM);

		digitalWrite(dc, HIGH);
		digitalWrite(cs, LOW);

#if defined (ESP8266) || defined (ESP32)
		spi->writeBytes((uint8_t *)buffer, PHYSWIDTH * PHYSHEIGHT / 8);
#elif defined (__STM32F1__)
		spi->write((const void *)buffer, PHYSWIDTH * PHYSHEIGHT / 8);
#else
		for (uint16_t i = 0; i < (PHYSWIDTH * PHYSHEIGHT / 8); i++)
			spi->transfer(buffer[i]);
#endif
		digitalWrite(cs, HIGH);

		_sendCommand(WINK_DISPLAY_UPDATE_CONTROL_2);
		_sendData(0xC4);
		_sendCommand(WINK_MASTER_ACTIVATION);
		_sendCommand(WINK_TERMINATE_FRAME_READ_WRITE);
		spi->endTransaction();
		if(waitUntilFinished) busyWait();
	}
}

void wInkDisplay::setMemoryArea(int16_t x_start, int16_t y_start, int16_t x_end, int16_t y_end) {
	if(tricolour) return;
	busyWait();
	spi->beginTransaction(spiSettings);
	_setMemoryArea(x_start, y_start, x_end, y_end);
	spi->endTransaction();
}

void wInkDisplay::_setMemoryArea(int16_t x_start, int16_t y_start, int16_t x_end, int16_t y_end) {
	if(tricolour) return;
	_sendCommand(WINK_SET_RAM_X_ADDRESS_START_END_POSITION);
	_sendData((x_start >> 3) & 0xFF);
	_sendData((x_end >> 3) & 0xFF);
	_sendCommand(WINK_SET_RAM_Y_ADDRESS_START_END_POSITION);
	_sendData(y_start & 0xFF);
	_sendData((y_start >> 8) & 0xFF);
	_sendData(y_end & 0xFF);
	_sendData((y_end >> 8) & 0xFF);
}

void wInkDisplay::setMemoryPointer(int16_t x, int16_t y) {
	if(tricolour) return;
	busyWait();
	spi->beginTransaction(spiSettings);
	_setMemoryPointer(x, y);
	spi->endTransaction();
}

void wInkDisplay::_setMemoryPointer(int16_t x, int16_t y) {
	if(tricolour) return;
	_sendCommand(WINK_SET_RAM_X_ADDRESS_COUNTER);
	_sendData((x >> 3) & 0xFF);
	_sendCommand(WINK_SET_RAM_Y_ADDRESS_COUNTER);
	_sendData(y & 0xFF);
	_sendData((y >> 8) & 0xFF);
	busyWait();
}

void wInkDisplay::sleep() {
	spi->beginTransaction(spiSettings);
	if(tricolour){
		_sendCommand(WINK_TRI_DEEP_SLEEP);
		_sendData(0xa5);
	} else
		_sendCommand(WINK_DEEP_SLEEP_MODE);
	spi->endTransaction();
}

void wInkDisplay::wakeUp(){
	busyWait();
	begin();
}

void wInkDisplay::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t colour) {
	switch(rotation) {
		case 0:
			drawFastHLineAbsolute(x, y, w, colour);
			break;
		case 1:
			paramSwapint16_t(x, y);
			x = PHYSWIDTH - x - 1;
			drawFastVLineAbsolute(x, y, w, colour);
			break;
		case 2:
			x = PHYSWIDTH - x - 1;
			y = PHYSHEIGHT - y - 1;
			x -= (w - 1);
			drawFastHLineAbsolute(x, y, w, colour);
			break;
		case 3:
			paramSwapint16_t(x, y);
			y = PHYSHEIGHT - y - 1;
			y -= (w - 1);
			drawFastVLineAbsolute(x, y, w, colour);
			break;
	}
}

void wInkDisplay::drawFastHLineAbsolute(int16_t x, int16_t y, int16_t w, uint16_t colour){
	int16_t i;
	uint8_t *pBuffer = NULL, *cBuffer = NULL, mask;
	static uint8_t premask[8] = {0xFF, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
    static uint8_t postmask[8] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };

	if(x >= PHYSWIDTH || y >= PHYSHEIGHT || y < 0 || w <= 0) return;
	if(buffer == NULL) return;

	if(x < 0){
		w += x;
		x = 0;
	}
	if(x + w >= PHYSWIDTH)
		w = PHYSWIDTH - x;
	
	if(!tricolour && colour == WINK_COLOUR) return;

	pBuffer = buffer + ((y * PHYSWIDTH + x) / 8);
	if(tricolour) cBuffer = colourBuffer + ((y * PHYSWIDTH + x) / 8);
	if(x & 7) {
		i = 8 - (x & 7);
		mask = premask[i];
		if(w < i)
			mask &= (0XFF << (i - w + 1));
		switch (colour){
			case WINK_WHITE:
				*pBuffer |= mask;
				if(tricolour) *cBuffer |= mask;
				break;
			case WINK_BLACK:
				*pBuffer &= ~mask;
				if(tricolour) *cBuffer |= mask;
				break;
			case WINK_COLOUR:
				*cBuffer &= ~mask;
				*pBuffer |= mask;
				break;
			case WINK_INVERSE:
				*pBuffer ^= mask;
				if(tricolour) *cBuffer |= mask;
				break;
		}
		if(w < i)
			return;
		w -= i;
		pBuffer++;
		if(tricolour) cBuffer++;
	}
	if(w >= 8) {
		switch(colour){
			case WINK_INVERSE:
				do {
					*pBuffer=~(*pBuffer);
					pBuffer++;
					if(tricolour){
						*cBuffer = 0xFF;
						cBuffer++;
					}
					w -= 8;
				} while(w >= 8);
				break;
			case WINK_WHITE:
				do {
					*pBuffer = 0xFF;
					pBuffer++;
					if(tricolour){
						*cBuffer = 0xFF;
						cBuffer++;
					}
					w -= 8;
				} while(w >= 8);
				break;
			case WINK_BLACK:
				do {
					*pBuffer = 0x00;
					pBuffer++;
					if(tricolour){
						*cBuffer = 0xFF;
						cBuffer++;
					}
					w -= 8;
				} while(w >= 8);
				break;
			case WINK_COLOUR:
				do {
					*pBuffer = 0xFF;
					pBuffer++;
					*cBuffer = 0x00;
					cBuffer++;
					w -= 8;
				} while(w >= 8);
				break;
		}
	}
	if(w){
		i = w & 7;
		mask = postmask[i];
		switch (colour){
			case WINK_WHITE:
				*pBuffer |= mask;
				if(tricolour) *cBuffer |= mask;
				break;
			case WINK_BLACK:
				*pBuffer &= ~mask;
				if(tricolour) *cBuffer |= mask;
				break;
			case WINK_COLOUR:
				*cBuffer &= ~mask;
				*pBuffer |= mask;
				break;
			case WINK_INVERSE:
				*pBuffer ^= mask;
				if(tricolour) *cBuffer |= mask;
				break;
		}
	}
}

void wInkDisplay::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t colour){
	switch(rotation) {
		case 0:
			drawFastVLineAbsolute(x, y, h, colour);
			break;
		case 1:
			paramSwapint16_t(x, y);
			x = PHYSWIDTH - x - 1;
			x -= (h - 1);
			drawFastHLineAbsolute(x, y, h, colour);
			break;
		case 2:
			x = PHYSWIDTH - x - 1;
			y = PHYSHEIGHT - y - 1;
			y -= (h - 1);
			drawFastVLineAbsolute(x, y, h, colour);
			break;
		case 3:
			paramSwapint16_t(x, y);
			y = PHYSHEIGHT - y - 1;
			drawFastHLineAbsolute(x, y, h, colour);
			break;
	}
}

void wInkDisplay::drawFastVLineAbsolute(int16_t x, int16_t y, int16_t h, uint16_t colour) {
	uint8_t *pBuffer, *cBuffer, mask, cmask;
	if(x < 0 || x >= PHYSWIDTH)
		return;
	if(y < 0) {
		h += y;
		y = 0;
	}
	if( (y + h) > PHYSHEIGHT) {
		h = (PHYSHEIGHT - y);
	}
	if(h <= 0)
		return;
	pBuffer = buffer + ((y * PHYSWIDTH + x) / 8);
	if(tricolour) cBuffer = colourBuffer + ((y * PHYSWIDTH + x) / 8);
	mask = 0x80 >> (x&7);
	switch (colour){
		case WINK_WHITE:
			while(h--){
				*pBuffer |= mask;
				pBuffer += PHYSWIDTH / 8;
				if(tricolour){
					*cBuffer |= mask;
					cBuffer += PHYSWIDTH / 8;
				}
			}
			break;
		case WINK_BLACK:
			cmask = mask;
			mask = ~mask;
			while(h--){
				*pBuffer &= mask;
				pBuffer += PHYSWIDTH / 8;
				if(tricolour){
					*cBuffer |= cmask;
					cBuffer += PHYSWIDTH / 8;
				}
			}
			break;
		case WINK_COLOUR:
			if(!tricolour) break;
			cmask = ~mask;
			while(h--){
				*cBuffer &= cmask;
				cBuffer += PHYSWIDTH / 8;
				*pBuffer |= mask;
				pBuffer += PHYSWIDTH / 8;
			}
			break;
		case WINK_INVERSE:
			while(h--){
				*pBuffer ^= mask;
				pBuffer += PHYSWIDTH / 8;
				if(tricolour){
					*cBuffer |= mask;
					cBuffer += PHYSWIDTH / 8;
				}
			}
			break;
	}
}

void wInkDisplay::fillScreen(uint16_t colour) {
	clearDisplay(colour);
}