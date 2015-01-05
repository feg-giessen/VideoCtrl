/*
 * LedController.h
 *
 *  Created on: 03.01.2015
 *      Author: Peter
 */

#ifndef LEDCONTROLLER_H_
#define LEDCONTROLLER_H_

#include "hw/PCA9685.h"
#include "I2cBus.h"

class LedController {
private:
    uint8_t _address;
    PCA9685  _ctrl;

    bool _isOnline;
    uint8_t _cache[16];

public:
    LedController();
    bool begin(I2cBus* bus, int address);
    void writeLed(uint8_t number, uint8_t value);
};

#endif /* LEDCONTROLLER_H_ */
