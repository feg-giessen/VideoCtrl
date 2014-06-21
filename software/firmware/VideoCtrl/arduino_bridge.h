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

unsigned long millis() {
    systime_t ticks = chTimeNow();

    return (((ticks - 1UL) * 1000UL) + 1UL) / CH_FREQUENCY;
}

#endif /* ARDUINO_BRIDGE_H_ */
