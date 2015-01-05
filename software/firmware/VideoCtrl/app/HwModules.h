/*
 * HwModules.h
 *
 *  Created on: 27.12.2013
 *      Author: Peter Schuster
 */

#ifndef HWMODULES_H_
#define HWMODULES_H_

#include <stddef.h>
#include <stdint.h>
#include "../lib/I2cBus.h"
#include "../lib/hw/MCP24AA04.h"
#include "../lib/SkaarhojBI8.h"
#include "../lib/Display.h"
#include "../lib/LedController.h"
#include "ReaderThread.h"

#define NUMBER_BI8 7

class HwModules {
private:
    I2cBus _i2cBus1;
    I2cBus _i2cBus2;

    Display _display;
    bool _display_online;

    LedController _ledCtrl;

    MCP24AA04 _eeprom;

    SkaarhojBI8 _bi8[NUMBER_BI8];
    bool _bi8_online[NUMBER_BI8];
public:
    HwModules();
    void init();
    void setScheduler(ReaderThread* scheduler);

    Display* getDisplay();
    MCP24AA04* getEeprom();
    SkaarhojBI8* getBi8(uint8_t number);
    LedController* getLedCtrl();
};

#endif /* HWMODULES_H_ */
