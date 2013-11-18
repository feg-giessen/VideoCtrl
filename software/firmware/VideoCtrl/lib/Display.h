/*
 * Display.h
 *
 *  Created on: 29.10.2013
 *      Author: Peter Schuster
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "hal.h"
#include "glcd.h"
#include "hw/EaDogL.h"
#include "I2cBus.h"
#include "hw/MCP23017.h"
#include "Buttons.h"
#include "Encoder.h"

#define DISP_B1_BIT       3
#define DISP_B2_BIT       1
#define DISP_B3_BIT       0
#define DISP_B4_BIT       2
#define DISP_B1_LED_BIT   7
#define DISP_B2_LED_BIT   5
#define DISP_B3_LED_BIT   4
#define DISP_B4_LED_BIT   6
#define DISP_ENC1_A_BIT  11
#define DISP_ENC1_B_BIT  12
#define DISP_ENC2_A_BIT  14
#define DISP_ENC2_B_BIT  15

class Display : public Buttons {
private:
	EaDogL*     _eaDogL;
	MCP23017*   _buttons;

	uint16_t    _dataReg;
	Encoder     _enc1, _enc2;

public:
	Display(SPIDriver* spip, I2cBus* i2cBus);
	void init();

	bool getButtonLed(int buttonNumber);
	void setButtonLed(int buttonNumber, bool on);

	int8_t getEncoder1(bool reset);
    int8_t getEncoder2(bool reset);

	virtual void readButtonStatus();

protected:
  virtual bool _validButtonNumber(int buttonNumber);
};

#endif /* DISPLAY_H_ */
