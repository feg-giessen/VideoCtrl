/*
 * MessageWriter.h
 *
 *  Created on: 02.02.2014
 *      Author: Peter Schuster
 */

#ifndef MESSAGEWRITER_H_
#define MESSAGEWRITER_H_

#include "../lib/Display.h"

#if !defined(DISPLAY_LINES)
#define DISPLAY_LINES 8
#endif

class MessageWriter {
private:
	Display* _display;
	uint8_t _line;
public:
	MessageWriter();

	void begin(Display* display);
	void write(const char* text);
	void reset();
};

#endif /* MESSAGEWRITER_H_ */
