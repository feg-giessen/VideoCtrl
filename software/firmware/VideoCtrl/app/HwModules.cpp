/*
 * HwModules.cpp
 *
 *  Created on: 27.12.2013
 *      Author: Peter Schuster
 */

#include "HwModules.h"

HwModules::HwModules() {
}

void HwModules::init() {
    _i2cBus1.begin(&I2CD1);
    _i2cBus2.begin(&I2CD2);

    _display_online = _display.begin(&SPID1, &_i2cBus2);
    _eeprom.begin(&_i2cBus2);

    _ledCtrl.begin(&_i2cBus2, 0);

    int i;
    for (i = 0; i < NUMBER_BI8; i++) {
        _bi8_online[i] = _bi8[i].begin(&_i2cBus1, i + 1); // addresses are from 1 to 7 !
    }
}

void HwModules::setScheduler(ReaderThread* scheduler) {

    scheduler->add(&_display, 5);

    int i;
    for (i = 0; i < NUMBER_BI8; i++) {
        scheduler->add(&_bi8[i], 3);
    }
}

Display* HwModules::getDisplay() {
    return &_display;
}

MCP24AA04* HwModules::getEeprom() {
    return &_eeprom;
}

SkaarhojBI8* HwModules::getBi8(uint8_t number) {
    if (number >= NUMBER_BI8)
        return NULL;

    return &_bi8[number];
}

LedController* HwModules::getLedCtrl() {
    return &_ledCtrl;
}
