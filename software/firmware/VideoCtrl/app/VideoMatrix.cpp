/*
 * VideoMatrix.cpp
 *
 *  Created on: 07.02.2014
 *      Author: Peter Schuster
 */

#include "VideoMatrix.h"

VideoMatrix::VideoMatrix() {
	uint8_t output, input;

	for (output = 0; output < 4; output++) {
		for (input = 0; input < 4; input++) {
			_button_boards[output][input] = NULL;
			_button_numbers[output][input] = 0;
		}
	}
}


void VideoMatrix::begin(ip_addr_t addr, uint16_t port) {
	_switch.begin(addr, port);
}

void VideoMatrix::setButton(uint8_t input, uint8_t output, SkaarhojBI8* button, uint8_t number) {
	if (input > 4 || output > 4 || input < 1 || output < 1)
		return;

	_button_boards[output - 1][input - 1] = button;
	_button_numbers[output - 1][input - 1] = number;
}

void VideoMatrix::run() {
	uint8_t output, input;

	for (output = 0; output < 4; output++) {
		uint8_t pressed = 0;
		for (input = 0; input < 4; input++) {
			if (_buttonDown(output, input)) {
				pressed = input + 1;
			}
		}

		if (pressed > 0) {
			_switch.setInput(output + 1, pressed);
		}
	}


	for (output = 0; output < 4; output++) {
		uint8_t current = _switch.getInput(output + 1);

		for (input = 0; input < 4; input++) {
			if (current == (input + 1)) {
				_setLed(output, input, LED_SELECTED);
			} else {
				_setLed(output, input, LED_NOT_SELECTED);
			}
		}
	}
}

void VideoMatrix::_setLed(uint8_t output, uint8_t input, uint8_t color) {
	if (output > 3 || input > 3)
		return;

	if (_button_boards[output][input] == NULL)
		return;

	_button_boards[output][input]->setButtonColor(
		_button_numbers[output][input],
		color);
}

bool VideoMatrix::_buttonDown(uint8_t output, uint8_t input) {
	if (output > 3 || input > 3)
		return false;

	if (_button_boards[output][input] == NULL)
		return false;

	return _button_boards[output][input]->buttonDown(_button_numbers[output][input]);
}
