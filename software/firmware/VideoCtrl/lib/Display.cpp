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
}
