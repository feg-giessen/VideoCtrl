/*  PCA9685 library for Arduino
    Copyright (C) 2012 Kasper Skårhøj    <kasperskaarhoj@gmail.com> 

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PCA9685_H
#define PCA9685_H

#include "I2cBus.h"

/**
  Version 1.0.0
	(Semantic Versioning)
**/
 
//Register defines from data sheet
#define PCA9685_MODE1 0x00 // location for Mode1 register address
#define PCA9685_MODE2 0x01 // location for Mode2 reigster address
#define PCA9685_LED0 0x06 // location for start of LED0 registers

#define PCA9685_I2C_BASE_ADDRESS 0b1000000

class PCA9685
{
  public:
    //NB the i2c address here is the value of the A0, A1, A2, A3, A4 and A5 pins ONLY
    //as the class takes care of its internal base address.
    //so i2cAddress should be between 0 and 63
    PCA9685();
    void begin(I2cBus* bus, uint8_t i2cAddress);
    bool init();

	void setLEDOn(uint8_t ledNumber);
	void setLEDOff(uint8_t ledNumber);
	void setLEDDimmed(uint8_t ledNumber, uint8_t amount);
	void writeLED(uint8_t ledNumber, uint16_t outputStart, uint16_t outputEnd);
	
  private:
	void writeRegister(uint8_t regaddress, uint8_t val);
	uint16_t readRegister(uint8_t regAddress);
	// Our actual i2c address:
	I2cBus* _bus;
	uint16_t _i2cAddress;
};
#endif 
