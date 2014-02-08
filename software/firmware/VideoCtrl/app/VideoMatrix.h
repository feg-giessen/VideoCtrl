/*
 * VideoMatrix.h
 *
 *  Created on: 07.02.2014
 *      Author: Peter Schuster
 */

#ifndef VIDEOMATRIX_H_
#define VIDEOMATRIX_H_

#include "../lib/MatrixSwitch.h"
#include "../lib/SkaarhojBI8.h"

#define LED_SELECTED BI8_COLOR_GREEN
#define LED_NOT_SELECTED BI8_COLOR_BACKLIGHT
#define LED_CONNECTION_LOST BI8_COLOR_OFF

class VideoMatrix {
private:
	MatrixSwitch _switch;

	SkaarhojBI8* _button_boards[4][4];
	uint8_t 	 _button_numbers[4][4];
public:
	VideoMatrix();
	void begin(ip_addr_t addr, uint16_t port);

	void setButton(uint8_t input, uint8_t output, SkaarhojBI8* button, uint8_t number);

	void run();

private:
	void _setLed(uint8_t output, uint8_t input, uint8_t color);
	bool _buttonDown(uint8_t output, uint8_t input);
};

#endif /* VIDEOMATRIX_H_ */
