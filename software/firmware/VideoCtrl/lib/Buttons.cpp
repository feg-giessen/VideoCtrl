/*
 * Buttons.cpp
 *
 *  Created on: 13.11.2013
 *      Author: Peter Schuster
 */

#include "Buttons.h"

bool Buttons::buttonUp(int buttonNumber) {
    if (_validButtonNumber(buttonNumber)) {

        uint16_t mask = (0b1 << (buttonNumber-1));
        uint16_t buttonChange = (_buttonStatusLastUp ^ _buttonStatus) & mask;
        _buttonStatusLastUp ^= buttonChange;

        return (buttonChange & ~_buttonStatus) ? true : false;
    } else return false;
}

bool Buttons::buttonDown(int buttonNumber) {
    if (_validButtonNumber(buttonNumber)) {

        uint16_t mask = (0b1 << (buttonNumber-1));
        uint16_t buttonChange = (_buttonStatusLastDown ^ _buttonStatus) & mask;
        _buttonStatusLastDown ^= buttonChange;

        return (buttonChange & _buttonStatus) ? true : false;
    } else return false;
}

void Buttons::handleDown(int buttonNumber) {
    if (_validButtonNumber(buttonNumber)) {

        uint16_t mask = (0b1 << (buttonNumber-1));
        _buttonStatusLastDown = _buttonStatusLastDown | mask;
    }
}

void Buttons::handleUp(int buttonNumber) {
    if (_validButtonNumber(buttonNumber)) {

        uint16_t mask = (0b1 << (buttonNumber-1));
        _buttonStatusLastUp = _buttonStatusLastUp | mask;
    }
}

bool Buttons::buttonIsPressed(int buttonNumber) {
    if (_validButtonNumber(buttonNumber)) {
        return (_buttonStatus >> (buttonNumber-1)) & 1 ? true : false;
    } else return false;
}
