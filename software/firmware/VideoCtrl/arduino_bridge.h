/*
 * arduino_bridge.h
 *
 *  Created on: 10.09.2013
 *      Author: Peter Schuster
 */

#ifndef ARDUINO_BRIDGE_H_
#define ARDUINO_BRIDGE_H_

#include "chvt.h"
#include "hal.h"

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define B 0b

unsigned long millis() {
	return RTT2MS(chTimeNow());
}

#endif /* ARDUINO_BRIDGE_H_ */
