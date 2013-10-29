/*
 * Display.cpp
 *
 *  Created on: 29.10.2013
 *      Author: Peter Schuster
 */

#include "Display.h"
#include "chthreads.h"

Display::Display(SPIDriver *spip, I2cBus *i2cBus) {
	_eaDogL = new EaDogL(spip, GPIOD, GPIOD_PIN0);
	_buttons = new MCP23017();
	_buttons->begin(i2cBus, 0);
}

void Display::init() {
	_buttons->init();
	_buttons->inputOutputMask(0xFF0F);
	_buttons->internalPullupMask(0xFFFF);
	_buttons->inputPolarityMask(0xFFFF);

	_buttons->digitalWordWrite(0xFF0F);

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

	uint8_t data0[] = { 0x00, 0x00, 0x00, 0x00 };
	uint8_t data1[] = { 0xFF, 0xFF, 0xFF, 0xFF };

	for (int i = 0; i < 2000; i++) {
		chThdSleepMilliseconds(10);
		uint8_t* data = i % 2 == 0 ? data0 : data1;
		_eaDogL->writeData(data, sizeof(data));
	}
}
