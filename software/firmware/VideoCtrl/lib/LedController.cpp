/*
 * LedController.cpp
 *
 *  Created on: 03.01.2015
 *      Author: Peter
 */

#include "LedController.h"
#include <string.h>

LedController::LedController() {
    memset(&_cache, 0, 16);
    _isOnline = false;
}

bool LedController::begin(I2cBus* bus, int address) {
    _address = (int)(0b110000 | (address & 0x0F));
    _ctrl.begin(bus, _address);

    memset(&_cache, 101, 16);
    _isOnline = false;

    _isOnline = _ctrl.init();

    return _isOnline;
}

void LedController::writeLed(uint8_t number, uint8_t value)  {
    chDbgAssert(
            _isOnline == true,
            "LedController::writeLed",
            "Not online.");

    if (number < 16 && value <= 100 && _cache[number] != value) {
        _cache[number] = value;
        _ctrl.setLEDDimmed(number, value);
    }
}
