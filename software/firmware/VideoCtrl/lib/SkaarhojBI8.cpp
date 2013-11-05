/*  SkaarhojBI8 Arduino library for the BI8 board from SKAARHOJ.com
    Copyright (C) 2012 Kasper Skårhøj    <kasperskaarhoj@gmail.com> 

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ch.h"
#include "SkaarhojBI8.h"


SkaarhojBI8::SkaarhojBI8(){
	_debugMode = false;
	_oldBI8 = false;
}	// Empty constructor.

bool SkaarhojBI8::begin(I2cBus* bus, int address)	{
	return begin(bus, address, false);
}
bool SkaarhojBI8::begin(I2cBus* bus, int address, bool reverseButtons) {
	// NOTE: Wire.h should definitely be initialized at this point! (Wire.begin())
	
	
	_boardAddress = (address & 0b111);	// 0-7
	_reverseButtons = reverseButtons;	// If set, buttons on the PCB is mounted on the bottom (opposite side of the chips). This affects how the LEDs should be programmed. All button numbers are the same. (OBSOLETE!)	
	_B1Alt = false;	
	_RGBbuttons = false;

		// Initializing:
	_buttonStatus = 0;
	_buttonStatusLastUp = 0;
	_buttonStatusLastDown = 0;
		
	setButtonType(0);	// Assuming NKK buttons as default
	
		// Used to track last used color in order to NOT write colors to buttons if they already have that color (writing same color subsequently will make the LED blink weirdly because each time a new timing scheme is randomly created in the PCA9685)
	_buttonColorCache[0] = 255;
	_buttonColorCache[1] = 255;
	_buttonColorCache[2] = 255;
	_buttonColorCache[3] = 255;
	_buttonColorCache[4] = 255;
	_buttonColorCache[5] = 255;
	_buttonColorCache[6] = 255;
	_buttonColorCache[7] = 255;
	_buttonColorCache[8] = 255;
	_buttonColorCache[9] = 255;
	
	_defaultColorNumber = 5;
		
		// Create object for reading button presses
	_buttonMux.begin(bus, (int)_boardAddress);

		// Create object for writing LED levels:
	_buttonLed.begin(bus, (int)(0b111000 | _boardAddress));
	
		// Inputs:
	_buttonMux.inputPolarityMask(0);
	bool isOnline = _buttonMux.init();

		// Set everything as inputs with pull up resistors:
	_buttonMux.internalPullupMask(65535);	// All has pull-up
	_buttonMux.inputOutputMask(65535);	// All are inputs.
//	delay(10);
	uint16_t buttonStatus = _buttonMux.digitalWordRead();	// Read out.
//	Serial.println(buttonStatus, BIN);
	if ((buttonStatus & 1) == 0)  {	// Test value of GPB0
		 _buttonMux.inputPolarityMask(65535);
	} else if ((buttonStatus >> 8) < 255) {	// Test if any of GPA0-7 are low (indicating pull-down resistors of 10K - or a button press!! Could be refined to test for more than one press)
		_buttonMux.internalPullupMask(0);	// In this case we don't need pull-up resistors...
		setButtonType(1);	// Assuming E-switch buttons for old BI8 boards
		_oldBI8 = true;
	} else {
		_buttonMux.inputOutputMask(0);	// We configure everything as outputs...
		setButtonType(1);	// Assuming E-switch buttons for old BI8 boards
		_oldBI8 = true;
	}

		// Outputs:
	_buttonLed.init();	
	setButtonColorsToDefault();
	
	return isOnline;
}
void SkaarhojBI8::usingB1alt()	{
	_B1Alt=true;	
}
bool SkaarhojBI8::isOnline() {
	return _buttonMux.init();	// It's not necessary to init the board for this - but it doesn't harm and is most easy...
}
bool SkaarhojBI8::isRGBboard()	{
	return _RGBbuttons;
}
void SkaarhojBI8::debugMode()	{
	_debugMode=true;	
}
void SkaarhojBI8::setButtonType(uint8_t type)	{
	// Rate from 0-100 for color numbers: Off(0), On(1), Red(2), Green(3), Amber(4), Backlit(5), Blue 100% (6), ...., White(9)
	switch(type)	{
		case 1: // LP 11 from e-switch
		_colorBalanceRed[0] = 0;
		_colorBalanceRed[1] = 100;
		_colorBalanceRed[2] = 100;
		_colorBalanceRed[3] = 0;
		_colorBalanceRed[4] = 100;
		_colorBalanceRed[5] = 20;
		_colorBalanceRed[6] = 0;
		_colorBalanceRed[7] = 0;
		_colorBalanceRed[8] = 0;
		_colorBalanceRed[9] = 0;

		_colorBalanceGreen[0] = 0;
		_colorBalanceGreen[1] = 100;
		_colorBalanceGreen[2] = 0;
		_colorBalanceGreen[3] = 100;
		_colorBalanceGreen[4] = 50;
		_colorBalanceGreen[5] = 10;
		_colorBalanceGreen[6] = 0;
		_colorBalanceGreen[7] = 0;
		_colorBalanceGreen[8] = 0;
		_colorBalanceGreen[9] = 0;		
		break;
		default: 	// NKK buttons
		_colorBalanceRed[0] = 0;
		_colorBalanceRed[1] = 70;
		_colorBalanceRed[2] = 70;
		_colorBalanceRed[3] = 0;
		_colorBalanceRed[4] = 30;
		_colorBalanceRed[5] = 5;
		_colorBalanceRed[6] = 0;
		_colorBalanceRed[7] = 0;
		_colorBalanceRed[8] = 0;
		_colorBalanceRed[9] = 100;

		_colorBalanceGreen[0] = 0;
		_colorBalanceGreen[1] = 70;
		_colorBalanceGreen[2] = 0;
		_colorBalanceGreen[3] = 70;
		_colorBalanceGreen[4] = 30;
		_colorBalanceGreen[5] = 5;
		_colorBalanceGreen[6] = 0;
		_colorBalanceGreen[7] = 0;
		_colorBalanceGreen[8] = 0;
		_colorBalanceGreen[9] = 100;		

		_colorBalanceBlue[0] = 0;
		_colorBalanceBlue[1] = 0;
		_colorBalanceBlue[2] = 0;
		_colorBalanceBlue[3] = 0;
		_colorBalanceBlue[4] = 0;
		_colorBalanceBlue[5] = 0;
		_colorBalanceBlue[6] = 100;
		_colorBalanceBlue[7] = 50;
		_colorBalanceBlue[8] = 20;
		_colorBalanceBlue[9] = 100;		
		break;
	}
}
void SkaarhojBI8::setColorBalance(int colorNumber, int redPart, int greenPart) {
	if (_validColorNumber(colorNumber) && _validPercentage(redPart) && _validPercentage(greenPart))	{
		_colorBalanceRed[colorNumber] = redPart;
		_colorBalanceGreen[colorNumber] = greenPart;
	}
}
void SkaarhojBI8::setColorBalanceRGB(int colorNumber, int redPart, int greenPart, int bluePart) {
	if (_validColorNumber(colorNumber) && _validPercentage(redPart) && _validPercentage(greenPart) && _validPercentage(bluePart))	{
		_colorBalanceRed[colorNumber] = redPart;
		_colorBalanceGreen[colorNumber] = greenPart;
		_colorBalanceBlue[colorNumber] = bluePart;
	}
}
void SkaarhojBI8::setDefaultColor(int defaultColorNumber) {
	if (_validColorNumber(defaultColorNumber))	_defaultColorNumber = defaultColorNumber;
}
void SkaarhojBI8::setButtonColor(int buttonNumber, int colorNumber) {
	_writeButtonLed(buttonNumber,colorNumber);
}
void SkaarhojBI8::setButtonColorsToDefault() {
	for(int i=1;i<=10;i++)  {
	  _writeButtonLed(i,_defaultColorNumber);
	}
}
void SkaarhojBI8::testSequence() { testSequence(20); }
uint16_t SkaarhojBI8::testSequence(int delayTime) {
  uint16_t bDown = 0;
	// Test LEDS:
  for(int ii=0;ii<=9;ii++)  {
    for(int i=1;i<=10;i++)  {
      _writeButtonLed(i,ii);
			// Test for button press and exit if so:
	  bDown = buttonDownAll();
	  if (bDown)	{
		return bDown;
	  }
	  chThdSleepMilliseconds(delayTime);
    }
    chThdSleepMilliseconds(delayTime*3);
  }
	setButtonColorsToDefault();
	return 0;
}


bool SkaarhojBI8::buttonUp(int buttonNumber) {	// Returns true if a button 1-10 is has just been released
	if (_validButtonNumber(buttonNumber))	{
		_readButtonStatus();

		uint16_t mask = (0b1 << (buttonNumber-1));
		uint16_t buttonChange = (_buttonStatusLastUp ^ _buttonStatus) & mask;
		_buttonStatusLastUp ^= buttonChange;

		return (buttonChange & ~_buttonStatus) ? true : false;
	} else return false;
}
bool SkaarhojBI8::buttonDown(int buttonNumber) {	// Returns true if a button 1-10 is has just been pushed down
	if (_validButtonNumber(buttonNumber))	{
		_readButtonStatus();

		uint16_t mask = (0b1 << (buttonNumber-1));
		uint16_t buttonChange = (_buttonStatusLastDown ^ _buttonStatus) & mask;
		_buttonStatusLastDown ^= buttonChange;

		return (buttonChange & _buttonStatus) ? true : false;
	} else return false;
}
bool SkaarhojBI8::buttonIsPressed(int buttonNumber) {	// Returns true if a button 1-10 is currently pressed
	if (_validButtonNumber(buttonNumber))	{
		_readButtonStatus();
		return (SkaarhojBI8::buttonIsPressedAll() >> (buttonNumber-1)) & 1 ? true : false;
	} else return false;
}
uint16_t SkaarhojBI8::buttonUpAll() {	// Returns a word where each bit indicates if a button 1-10 (bits 0-9) has been released since last check
	_readButtonStatus();
	
	uint16_t buttonChange = _buttonStatusLastUp ^ _buttonStatus;
	_buttonStatusLastUp = _buttonStatus;

	return buttonChange & ~_buttonStatus;
}
uint16_t SkaarhojBI8::buttonDownAll() {	// Returns a word where each bit indicates if a button 1-10 (bits 0-9) has been pressed since last check
	_readButtonStatus();
	
	uint16_t buttonChange = _buttonStatusLastDown ^ _buttonStatus;
	_buttonStatusLastDown = _buttonStatus;
	
	return buttonChange & _buttonStatus;
}
uint16_t SkaarhojBI8::buttonIsPressedAll() {	// Returns a word where each bit indicates if a button 1-10 (bits 0-9) is currently pressed since last check
	_readButtonStatus();
	
	return _buttonStatus;
}
bool SkaarhojBI8::isButtonIn(int buttonNumber, uint16_t allButtonsState)	{
	if (_validButtonNumber(buttonNumber))	{
		return (allButtonsState & (0b1 << (buttonNumber-1))) ? true : false;
	}
	return false;
}



// Private methods:

void SkaarhojBI8::_writeButtonLed(int buttonNumber, int color)  {
	if (_validColorNumber(color) && _validButtonNumber(buttonNumber) && _buttonColorCache[(buttonNumber-1)] != color)		{
		_buttonColorCache[(buttonNumber-1)] = color;
		
		uint8_t isNormal = _reverseButtons ? 0 : 1;
		if (!_RGBbuttons)	{
		    if((buttonNumber<=4 && !(buttonNumber==1 && _B1Alt)) || !_oldBI8)  {
		      _buttonLed.setLEDDimmed((buttonNumber-1)*2+1*(1-isNormal),  _colorBalanceRed[color]);
		      _buttonLed.setLEDDimmed((buttonNumber-1)*2+1*isNormal, _colorBalanceGreen[color]);
		    } else {
		      _buttonLed.setLEDDimmed((buttonNumber-1)*2+1*(1-isNormal),  _colorBalanceGreen[color]);
		      _buttonLed.setLEDDimmed((buttonNumber-1)*2+1*isNormal, _colorBalanceRed[color]);
		    }
		} else {
				// isNormal is assumed not used for RGB boards:
			if (buttonNumber<9)	{
				_buttonLed.setLEDDimmed((buttonNumber-1)*2,  _colorBalanceGreen[color]);
				_buttonLed.setLEDDimmed((buttonNumber-1)*2+1, _colorBalanceRed[color]);
			}
		}
	}
}

void SkaarhojBI8::_readButtonStatus() {	// Reads button status from MCP23017 chip.
	uint16_t buttonStatus = _buttonMux.digitalWordRead();
	_buttonStatus = buttonStatus >> 8;
	
	_buttonStatus = 
		((_buttonStatus & 0b10000) >> 4) | 	// B1
		((_buttonStatus & 0b100000) >> 4) | 	// B2
		((_buttonStatus & 0b1000000) >> 4) | 	// B3
		((_buttonStatus & 0b10000000) >> 4) | 	// B4
		((_buttonStatus & 0b1000) << 1) | 	// B5
		((_buttonStatus & 0b100) << 3) | 	// B6
		((_buttonStatus & 0b10) << 5) | 	// B7
		((_buttonStatus & 0b1) << 7) |	// B8
		((buttonStatus & 0b10000000) << 1) |	// B9
		((buttonStatus & 0b1000000) << 3);	// B10
}

bool SkaarhojBI8::_validButtonNumber(int buttonNumber)	{	// Checks if a button number is valid (1-10)
	return (buttonNumber>=1 && buttonNumber <= 10);
}
bool SkaarhojBI8::_validColorNumber(int colorNumber)	{	// Checks if a color number is valid (1-10)
	return (colorNumber>=0 && colorNumber <= 5);
}
bool SkaarhojBI8::_validPercentage(int percentage)	{	// Checks if a value is within 0-100
	return (percentage>=0 && percentage <= 100);
}

