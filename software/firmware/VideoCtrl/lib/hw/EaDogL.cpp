/*
 * EaDogL.cpp
 *
 *  Created on: 29.10.2013
 *      Author: Peter Schuster
 */

#include "EaDogL.h"

EaDogL::EaDogL(SPIDriver* spip, GPIO_TypeDef* port_mode, uint16_t pad_mode) {
	_spip = spip;
	_port_mode = port_mode;
	_pad_mode = pad_mode;

	// We're always in 'selected' mode.
	spiSelect(spip);
}

void EaDogL::setOnOff(bool on) {
	setMode(EaDogL_Command);

	uint8_t data = 0b10101110 | (on == true ? 1 : 0);
	spiSend(_spip, 1, &data);
}

void EaDogL::setStartAddress(uint8_t address) {
	setMode(EaDogL_Command);

	uint8_t data = 0x40 | (0x3F & address);
	spiSend(_spip, 1, &data);
}

void EaDogL::setPageAddress(uint8_t address) {
	setMode(EaDogL_Command);

	uint8_t data = 0xB0 | (0x0F & address);
	spiSend(_spip, 1, &data);
}

void EaDogL::setColumnAddress(uint8_t address) {
	setMode(EaDogL_Command);

	// 4 MSB
	uint8_t data = 0x10 | (0x0F & (address >> 4));
	spiSend(_spip, 1, &data);

	// 4 LSB
	data = 0x00 | (0x0F & (address >> 0));
	spiSend(_spip, 1, &data);
}

void EaDogL::writeData(const uint8_t *data, int count) {
	setMode(EaDogL_Data);

	spiSend(_spip, count, data);
}

void EaDogL::setAdc(EaDogL_Adc adc) {
	setMode(EaDogL_Command);

	uint8_t data;

	switch (adc) {
	case Adc_Reverse:
		data = 0xA1;
		break;
	case Adc_Normal:
	default:
		data = 0xA0;
		break;
	}
	spiSend(_spip, 1, &data);
}

void EaDogL::setDisplay(EaDogL_DisplayMode mode) {
	setMode(EaDogL_Command);

	uint8_t data;

	switch (mode) {
	case DisplayMode_Reverse:
		data = 0xA7;
		break;
	case DisplayMode_Normal:
	default:
		data = 0xA6;
		break;
	}
	spiSend(_spip, 1, &data);
}

void EaDogL::setDsiplayAllPoints(bool allPoints) {
	setMode(EaDogL_Command);

	uint8_t data = allPoints == true ?  : 0xA5 | 0xA4;
	spiSend(_spip, 1, &data);
}

void EaDogL::setDriveVoltageBias(EaDogL_DriveVoltageBiasRatio ratio) {
	setMode(EaDogL_Command);

	uint8_t data;

	switch (ratio) {
	case DriveVoltageBiasRatio_1_7:
		data = 0xA3;
		break;
	case DriveVoltageBiasRatio_1_9:
	default:
		data = 0xA2;
		break;
	}
	spiSend(_spip, 1, &data);
}

void EaDogL::reset() {
	setMode(EaDogL_Command);

	uint8_t data = 0xE2;
	spiSend(_spip, 1, &data);
}

void EaDogL::setComOutput(EaDogL_ComOutput direction) {
	setMode(EaDogL_Command);

	uint8_t data;

	switch (direction) {
	case ComOutput_Reverse:
		data = 0xC8;
		break;
	case ComOutput_Normal:
	default:
		data = 0xC0;
		break;
	}
	spiSend(_spip, 1, &data);
}

void EaDogL::setPowerCtrlMode(uint8_t bits) {
	setMode(EaDogL_Command);

	uint8_t data = 0x28 | (0x7 & bits);
	spiSend(_spip, 1, &data);
}

void EaDogL::setPwrRegulatorResistorRatio(uint8_t value) {
	setMode(EaDogL_Command);

	uint8_t data = 0x20 | (0x7 & value);
	spiSend(_spip, 1, &data);
}

void EaDogL::setElectronicVolume(uint8_t value) {
	setMode(EaDogL_Command);

	uint8_t data[] = { 0x81, (uint8_t)(0x3F & value) };
	spiSend(_spip, sizeof(data), data);
}

void EaDogL::setStaticIndicator(uint8_t mode) {
	setMode(EaDogL_Command);

	uint8_t data = 0xAC | (mode == 0 ? 0 : 1);
	spiSend(_spip, 1, &data);

	if (mode != 0) {
		data = 0x3 & mode;
		spiSend(_spip, 1, &data);
	}
}

void EaDogL::setBoosterRatio(uint8_t ratio) {
	setMode(EaDogL_Command);

	uint8_t data[] = { 0xF8, (uint8_t)(0x3 & ratio) };
	spiSend(_spip, sizeof(data), data);
}

void EaDogL::setMode(EaDogL_CommandData mode) {
	if (mode == EaDogL_Command) {
		palClearPad(_port_mode, _pad_mode);	// set DISP_MODE <= 0
	} else if (mode == EaDogL_Data) {
		palSetPad(_port_mode, _pad_mode);	// set DISP_MODE <= 1
	}
}
