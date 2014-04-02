/*
 * PtzCamera.cpp
 *
 *  Created on: 02.02.2014
 *      Author: Peter Schuster
 */

#include <string.h>
#include <stdlib.h>
#include "PtzCamera.h"

PtzCamera::PtzCamera() {
	uint8_t i, l;
	_x = _y = _z = NULL;
	_setMem = false;
	_hasInitialized = false;

    // Initialize mapping
    l = (uint8_t)PTZ_enum_size;
    for (i = 0; i < l; i++) {
        _buttonBoardMapping[(PTZ_Functions)i]  = NULL;
        _buttonNumberMapping[(PTZ_Functions)i] = -1;
        _ledBoardMapping[(PTZ_Functions)i]     = NULL;
        _ledNumberMapping[(PTZ_Functions)i]    = -1;
    }

    // PTZ_None is not actually a function, but we ignore this fact for simplicity of the program.
}

void PtzCamera::begin(SerialDriver* sdp, AdcChannel* x, AdcChannel* y, AdcChannel* z) {
	_visca.begin(sdp);
	_x = x;
	_y = y;
	_z = z;
	_currentX = 0x00;
	_currentY = 0x00;
	_currentZ = 0x00;
	_yDirection = 0x00;
	_xDirection = 0x00;
	_focusAuto = true;
	_hasInitialized = false;
}

void PtzCamera::power(bool power) {
	_visca.setPower(power);
}

bool PtzCamera::online() {
	return false;
}

void PtzCamera::setButton(PTZ_Functions function, Buttons *buttons, const int number) {
    _buttonBoardMapping[function] = buttons;
    _buttonNumberMapping[function] = number;
}

void PtzCamera::setLed(PTZ_Functions function, SkaarhojBI8 *board, const int number) {
    _ledBoardMapping[function] = board;
    _ledNumberMapping[function] = number;
}

void PtzCamera::run() {

	// process incoming packets
	_visca.processPackets();

	// send auto replies (internal packets)
	_visca.autoReply();

	if (!_hasInitialized) {
		_visca.setInfoDisplay(false);
		_hasInitialized = true;
	}

	if (_focusAuto)
		_setLed(PTZ_None, BI8_COLOR_OFF);

	if (_buttonIsDown(PTZ_MEM_Store))
		_setMem = !_setMem;

	if (_setMem) {
		_setLed(PTZ_MEM_Store, BI8_COLOR_RED);
	} else {
		_setLed(PTZ_MEM_Store, BI8_COLOR_BACKLIGHT);
	}

	for (int i=1;i<=6;i++) {
		if (_buttonIsDown((PTZ_Functions)i)) {
			_visca.camMemory((PTZ_Functions)i, _setMem);
			if (_setMem) {
				_setMem = !_setMem;
				_setLed((PTZ_Functions)i, BI8_COLOR_RED);
				chThdSleepMilliseconds(500);
			}
			else
				_setLed((PTZ_Functions)i, BI8_COLOR_GREEN);

			chThdSleepMilliseconds(500);
			_setLed((PTZ_Functions)i, BI8_COLOR_BACKLIGHT);
		}
	}

	int _getX = 20 - _x->getValue() / 100;
	int _getY = 20 - _y->getValue() / 100;

	if (_getX < 2 && _getX > -2)
		_xDirection = 0x03;
	else if (_getX >= 2)
		_xDirection = 0x02;
	else if (_getX <= -2)
		_xDirection = 0x01;

	if (_getY < 2 && _getY > -2)
		_yDirection = 0x03;
	else if (_getY >= 2)
		_yDirection = 0x01;
	else if (_getY <= -2)
		_yDirection = 0x02;

	int _movement[2] = { abs(_getX), abs(_getY)};
	uint8_t _moveFinal[2];

	for (int i = 0; i<2; i++) {
		if ( _movement[i] % 2) {
			int test4 =  _movement[i] / 2;
			_moveFinal[i] = 0x00 | (test4 & 0x0F);;
		}
		else {
			if (i == 0)
				_moveFinal[i] = _currentX;
			if (i == 1)
				_moveFinal[i] = _currentY;
		}
	}

	if (_moveFinal[0] != _currentX || _moveFinal[1] != _currentY) {
		_visca.camDrive(_moveFinal[0], _moveFinal[1], _xDirection, _yDirection);
		_currentX = _moveFinal[0];
		_currentY = _moveFinal[1];
	}


	// ----- Z drive

	int _getZ = 20 - _z->getValue() / 100;
	uint8_t fo;

	switch (_getZ) {
		case -3:
			fo = 0x20;
			break;
		case -6:
			fo = 0x21;
			break;
		case -9:
			fo = 0x22;
			break;
		case -12:
			fo = 0x23;
			break;
		case -15:
			fo = 0x24;
			break;
		case -18:
			fo = 0x25;
			break;
		case 0:
			fo = 0x00;
			break;
		case 3:
			fo = 0x30;
			break;
		case 6:
			fo = 0x31;
			break;
		case 9:
			fo = 0x32;
			break;
		case 12:
			fo = 0x33;
			break;
		case 15:
			fo = 0x34;
			break;
		case 18:
			fo = 0x35;
			break;
		default:
			fo = _currentZ;
			break;
	}
	if (_buttonIsPressed(PTZ_None) && _setMem) {
		_setMem = !_setMem;
		_visca.camAuto();
		_focusAuto = true;
		_setLed(PTZ_None, BI8_COLOR_RED);
		chThdSleepMilliseconds(1000);
		_setLed(PTZ_None, BI8_COLOR_OFF);
	}
	if (fo != _currentZ) {
		if (!palReadPad(GPIOD, GPIOD_PIN14)) {
			if (_focusAuto)
				_visca.camMan();
				_focusAuto = false;
				_setLed(PTZ_None, BI8_COLOR_ON);

			_visca.camFocus(fo);
		}
		else
			_visca.camZoom(fo);

		_currentZ = fo;
	}
}

bool PtzCamera::_buttonIsPressed(PTZ_Functions function) {
    Buttons* board;
    int number;

    board  = _buttonBoardMapping[function];

    if (board == NULL)
        return false;

    number = _buttonNumberMapping[function];
    return board->buttonIsPressed(number);
}

bool PtzCamera::_buttonIsDown(PTZ_Functions function) {
    Buttons* board;
    int number;

    board  = _buttonBoardMapping[function];

    if (board == NULL)
        return false;

    number = _buttonNumberMapping[function];
    return board->buttonDown(number);
}

void PtzCamera::_setLed(PTZ_Functions function, int color) {
    SkaarhojBI8* board;
    int number;

    board  = _ledBoardMapping[function];

    if (board == NULL)
        return;

    number = _ledNumberMapping[function];
    return board->setButtonColor(number, color);
}