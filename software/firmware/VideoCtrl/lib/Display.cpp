/*
 * Display.cpp
 *
 *  Created on: 29.10.2013
 *      Author: Peter Schuster
 */

#include "ch.h"
#include <string.h>

extern "C" {
#include "glcd.h"
#include "glcd_graphs.h"
#include "glcd_controllers.h"
#include "glcd_text.h"
#include "glcd_text_tiny.h"
#include "font5x7.h"
#include "Bebas_Neue20x36_Bold_Numbers.h"
}

#include "Display.h"
#include "chthreads.h"
#include <string.h>

Display::Display() {
	_buttonStatus = 0;
	_buttonStatusLastDown = 0;
	_buttonStatusLastUp = 0;
}

bool Display::begin(SPIDriver *spip, I2cBus *i2cBus) {
    bool ret;

    _eaDogL = new EaDogL(spip, GPIOD, GPIOD_PIN0);
    _buttons = new MCP23017();

    _buttons->begin(i2cBus, 0);

    ret = _buttons->init();

    if (ret) {
        _buttons->inputOutputMask(0xFF0F);
        _buttons->internalPullupMask(0xFFFF);
        _buttons->inputPolarityMask(0xFFFF);

        _buttons->digitalWordWrite(0xFF0F);
    }

	_eaDogL->reset();
	_eaDogL->setStartAddress(0);
	_eaDogL->setAdc(Adc_Reverse);
	_eaDogL->setComOutput(ComOutput_Normal);
	_eaDogL->setDisplay(DisplayMode_Normal);
	_eaDogL->setDriveVoltageBias(DriveVoltageBiasRatio_1_9);
	_eaDogL->setPowerCtrlMode(EaDogL_Power_Booster | EaDogL_Power_Regulator | EaDogL_Power_Follower);
	_eaDogL->setPwrRegulatorResistorRatio(0x17);
	_eaDogL->setElectronicVolume(0x12);
	_eaDogL->setStaticIndicator(0);
	_eaDogL->setOnOff(true);

	uint8_t data[128];
	memset(data, 0, sizeof(data));

	for (uint8_t i = 0; i < 8; i++) {
		_eaDogL->setPageAddress(i);
		_eaDogL->setColumnAddress(0);
		_eaDogL->writeData(data, sizeof(data));
	}

	glcd_select_screen(glcd_buffer, &glcd_bbox);
	glcd_clear();
	glcd_clear_buffer();

	glcd_tiny_set_font(Font5x7,5,7,32,127);
	glcd_draw_string_xy(30,17,(char*)"FeG Giessen");
	glcd_draw_string_xy(20,40,(char*)"VideoController");

	glcd_write();

	return ret;
}

void Display::clear() {
	glcd_clear();
	glcd_clear_buffer();
	glcd_write();
}

void Display::writeText(char* text, uint8_t line) {
	glcd_draw_string_xy(0, (line *  8), text);
	glcd_write();
}

bool Display::getButtonLed(int buttonNumber) {

    if (!_validButtonNumber(buttonNumber))
        return false;

    uint16_t bits;
    switch (buttonNumber) {
        case 1:
            bits = (_dataReg >> DISP_B1_LED_BIT) & 0b1;
            break;
        case 2:
            bits = (_dataReg >> DISP_B2_LED_BIT) & 0b1;
            break;
        case 3:
            bits = (_dataReg >> DISP_B3_LED_BIT) & 0b1;
            break;
        case 4:
            bits = (_dataReg >> DISP_B4_LED_BIT) & 0b1;
            break;
        default:
            bits = 1;
            break;
    }

    return (bits == 0);
}

void Display::setButtonLed(int buttonNumber, bool on) {

    if (!_validButtonNumber(buttonNumber))
        return;

    uint16_t bits;

    switch (buttonNumber) {
        case 1:
            bits = 1 << DISP_B1_LED_BIT;
            break;
        case 2:
            bits = 1 << DISP_B2_LED_BIT;
            break;
        case 3:
            bits = 1 << DISP_B3_LED_BIT;
            break;
        case 4:
            bits = 1 << DISP_B4_LED_BIT;
            break;
        default:
            bits = 0;
            break;
    }

    // inverted logic (switched gnd)
    _dataReg = on ? _dataReg & ~bits : _dataReg | bits;

    _buttons->digitalWordWrite(_dataReg);
}

msg_t Display::readButtonStatus() {	// Reads button status from MCP23017 chip.
    uint16_t dataReg = _buttons->digitalWordRead();

    // Read buttons status bits
	_buttonStatus =
		(uint8_t)(((dataReg & (1 << DISP_B1_BIT)) >> DISP_B1_BIT))      |   // B1
        (uint8_t)(((dataReg & (1 << DISP_B2_BIT)) >> DISP_B2_BIT) << 1) |   // B2
        (uint8_t)(((dataReg & (1 << DISP_B3_BIT)) >> DISP_B3_BIT) << 2) |   // B3
        (uint8_t)(((dataReg & (1 << DISP_B4_BIT)) >> DISP_B4_BIT) << 3);    // B4

	_enc1.updateValue(dataReg, DISP_ENC1_A_BIT, DISP_ENC1_B_BIT);
	_enc2.updateValue(dataReg, DISP_ENC2_A_BIT, DISP_ENC2_B_BIT);

	_dataReg = dataReg;

	return 0; // TODO
}


int8_t Display::getEncoder1(bool reset) {
    return _enc1.getValue(reset);
}

int8_t Display::getEncoder2(bool reset) {
    return _enc2.getValue(reset);
}

bool Display::_validButtonNumber(int buttonNumber) {
    return (buttonNumber>=1 && buttonNumber <= 4);
}
