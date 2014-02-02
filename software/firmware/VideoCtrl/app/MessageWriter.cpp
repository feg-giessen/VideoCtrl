/*
 * MessageWriter.cpp
 *
 *  Created on: 02.02.2014
 *      Author: Peter Schuster
 */

#include "MessageWriter.h"

MessageWriter::MessageWriter() {
	_line = 0;
	_display = NULL;
}

void MessageWriter::begin(Display* display) {
	_line = 0;
	_display = display;
}

void MessageWriter::reset() {
	_line = 0;
}

void MessageWriter::write(const char* text) {

	if (_display == NULL) {
		chDbgPanic("MessageWriter not initialized (begin)");
	}

	if (_line == 0) {
		_display->clear();
	}

	_display->writeText((char*)text, _line);
	_line += 1;

	if (_line >= DISPLAY_LINES) {
		_line = 0;
	}
}
