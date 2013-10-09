/*
 * Stream.cpp
 *
 *  Created on: 29.09.2013
 *      Author: Peter Schuster
 */

#include "Stream.h"

Stream::Stream() {
	_ptr = NULL;
	_offset = 0;
	_len = 0;
}

Stream::~Stream() {
	_free();
}

void Stream::load(void* ptr, size_t len) {
	_ptr = ptr;
	_len = len;
	_offset = 0;
}

uint16_t Stream::read(uint8_t* &ptr, uint16_t bytes) {
	if (_len - _offset >= bytes) {
		ptr = (uint8_t*)((uint32_t)_ptr + _offset);
		_offset += bytes;

		return bytes;
	} else {
		uint16_t avail = _len - _offset;
		ptr = (uint8_t*)((uint32_t)_ptr + _offset);
		_offset += avail;

		return avail;
	}
}

uint16_t Stream::copy(uint8_t* ptr, uint16_t bytes) {
	uint16_t avail;

	memset(ptr, 0, bytes);

	if (_len - _offset >= bytes) {
		avail = bytes;
	} else {
		avail = _len - _offset;
	}

	memcpy((void*)ptr, (void*)((uint32_t)_ptr + _offset), avail);
	_offset += avail;

	return avail;
}

void Stream::flush() {
	_free();
	_len = 0;
	_offset = 0;
}

size_t Stream::getLength() {
	return _len;
}

uint16_t Stream::getOffset() {
	return _offset;
}

void Stream::reset() {
	_offset = 0;
}

void Stream::_free() {
	if (_ptr != NULL) {
		delete((uint8_t*)_ptr);
	}
}
