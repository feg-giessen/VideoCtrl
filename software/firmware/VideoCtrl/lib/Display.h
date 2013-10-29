/*
 * Display.h
 *
 *  Created on: 29.10.2013
 *      Author: Peter Schuster
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "hal.h"
#include "hw/EaDogL.h"
#include "I2cBus.h"
#include "hw/MCP23017.h"

class Display {
private:
	EaDogL* _eaDogL;
	MCP23017* _buttons;
public:
	Display(SPIDriver* spip, I2cBus* i2cBus);
	void init();
};

#endif /* DISPLAY_H_ */
