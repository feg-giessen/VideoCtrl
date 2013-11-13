/*
 * Buttons.h
 *
 *  Created on: 13.11.2013
 *      Author: Peter Schuster
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

#include <stdint.h>

class Buttons {
protected:
    uint16_t _buttonStatus;
    uint16_t _buttonStatusLastUp;
    uint16_t _buttonStatusLastDown;
public:
	virtual void readButtonStatus() = 0;
    bool buttonUp(int buttonNumber);
    bool buttonDown(int buttonNumber);
    bool buttonIsPressed(int buttonNumber);
protected:
    virtual bool _validButtonNumber(int buttonNumber) = 0;
};

#endif /* BUTTONS_H_ */
