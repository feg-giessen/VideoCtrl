/*
 * EaDogL.h
 *
 *  Created on: 29.10.2013
 *      Author: Peter Schuster
 */

#ifndef EADOGL_H_
#define EADOGL_H_

#include <stddef.h>
#include <stdint.h>
#include "hal.h"

enum EaDogL_CommandData {
	EaDogL_Command,
	EaDogL_Data
};

enum EaDogL_Adc {
	Adc_Normal,
	Adc_Reverse
};

enum EaDogL_DisplayMode {
	DisplayMode_Normal,
	DisplayMode_Reverse
};

enum EaDogL_DriveVoltageBiasRatio {
	DriveVoltageBiasRatio_1_9,
	DriveVoltageBiasRatio_1_7
};

enum EaDogL_ComOutput {
	ComOutput_Normal,
	ComOutput_Reverse
};

#define EaDogL_Power_Booster   (uint8_t)0b100
#define EaDogL_Power_Regulator (uint8_t)0b010
#define EaDogL_Power_Follower  (uint8_t)0b001

class EaDogL {
private:
	SPIDriver* _spip;
	GPIO_TypeDef* _port_mode;
	uint16_t _pad_mode;
public:
	EaDogL(SPIDriver* spip, GPIO_TypeDef* port_mode, uint16_t pad_mode);
	void setOnOff(bool on);
	void setStartAddress(uint8_t address);
	void setPageAddress(uint8_t address);;
	void setColumnAddress(uint8_t address);
	void writeData(const uint8_t* data, int count);
	void setAdc(EaDogL_Adc adc);
	void setDisplay(EaDogL_DisplayMode mode);
	void setDisplayAllPoints(bool allPoints);
	void setDriveVoltageBias(EaDogL_DriveVoltageBiasRatio ratio);
	void reset();
	void setComOutput(EaDogL_ComOutput direction);
	void setPowerCtrlMode(uint8_t bits);
	void setPwrRegulatorResistorRatio(uint8_t value); // alpha = 64 - (value + 1)
	void setElectronicVolume(uint8_t value);
	void setStaticIndicator(uint8_t mode); // 00 Off, 01 1sec, 10 0.5sec, 11 On
	void setBoosterRatio(uint8_t ratio);
private:
	void setMode(EaDogL_CommandData mode);
	void sendC(int count, const uint8_t* data);
	void sendD(int count, const uint8_t* data);
};

#endif /* EADOGL_H_ */
