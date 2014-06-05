/*
 * AdcChannel.cpp
 *
 *  Created on: 02.02.2014
 *      Author: Peter Schuster
 */

#include "AdcChannel.h"

AdcChannel::AdcChannel() {
	_buffer = NULL;
	_buffer_depth = 0;
	_count = 0;
	_number = 0;
	_invert = false;
}

void AdcChannel::begin(adcsample_t* buffer, uint8_t number, uint8_t ch_count, uint8_t buffer_depth, bool invert) {
	_buffer = buffer;
	_number = number;
	_count = ch_count;
	_buffer_depth = buffer_depth;
	_invert = invert;
}

uint16_t AdcChannel::getValue() {
	uint8_t i;
	uint16_t value = 0;

	for (i = 0; i < _buffer_depth; i++) {
		value += _buffer[_number + (i * _count)];
	}

	value = value / _buffer_depth;

	if (_invert) {
	    uint16_t inverter = 0;
	    inverter -= 1; // -> uint16_t.MAX_VALUE

	    return inverter - value;
	} else {
	    return value;
	}
}
