/*
 * Encoder.cpp
 *
 *  Created on: 18.11.2013
 *      Author: Peter Schuster
 */

#include "Encoder.h"
#include "ch.h"

const int8_t Encoder::_table[] = {0,0,-1,0,0,0,0,1,1,0,0,0,0,-1,0,0};

Encoder::Encoder() {
    _value = 0;
    _last = 0;
}

void Encoder::updateValue(uint16_t data, uint8_t bit_a, uint8_t bit_b) {
    _last = (_last << 2) & 0x0F;
    if (1 & (data >> bit_a)) _last |= 2;
    if (1 & (data >> bit_b)) _last |= 1;
    _value += _table[_last];
}

int8_t Encoder::getValue(bool reset) {
    int8_t result;

    if (reset) {
        chSysLock(); // atomic operation
        result = _value;
        _value = 0;
        chSysUnlock();
    } else {
        result = _value;
    }

    return result;
}
