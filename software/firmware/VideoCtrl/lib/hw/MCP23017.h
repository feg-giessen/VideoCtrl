/*  MCP23017 library for Arduino
    Copyright (C) 2009 David Pye    <davidmpye@gmail.com>
	Copyright (C) 2012 Kasper Skårhøj <kasperskaarhoj@gmail.com>

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
#ifndef MCP23017_H
#define MCP23017_H

/**
  Version 1.0.0
	(Semantic Versioning)
**/


// IMPORTANT on byte order:
// Note on Writing words: The MSB is for GPA7-0 and the LSB is for GPB7-0
// Pinnumbers 0-7 = GPB0-7, 8-15 = GPA0-7
// This comes across as slightly un-intuitive when programming


#include "../I2cBus.h"

//Register defines from data sheet - we set IOCON.BANK to 0
//as it is easier to manage the registers sequentially.
#define MCP23017_IODIR 0x00
#define MCP23017_IPOL  0x02
#define MCP23017_GPPU  0x0C
#define MCP23017_GPIO  0x12

#define MCP23017_I2C_BASE_ADDRESS 0x40

class MCP23017
{
  public:
    //NB the i2c address here is the value of the A0, A1, A2 pins ONLY
    //as the class takes care of its internal base address.
    //so i2cAddress should be between 0 and 7
    MCP23017();
    void begin(I2cBus* bus, uint8_t i2cAddress);
    bool init();

    //These functions provide an 'arduino'-like functionality for accessing
    //pin states/pullups etc.
    void pinMode(uint8_t pin, uint8_t mode);
    void digitalWrite(uint8_t pin, uint8_t val);
    int digitalRead(uint8_t pin);

    //These provide a more advanced mapping of the chip functionality
    //See the data sheet for more information on what they do

    //Returns a word with the current pin states (ie contents of the GPIO register)
    uint16_t digitalWordRead();

    //Allows you to write a word to the GPIO register
    void digitalWordWrite(uint16_t w);

    //Sets up the polarity mask that the MCP23017 supports
    //if set to 1, it will flip the actual pin value.
    void inputPolarityMask(uint16_t mask);

    //Sets which pins are inputs or outputs (1 = input, 0 = output) NB Opposite to arduino's
    //definition for these
    void inputOutputMask(uint16_t mask);

    //Allows enabling of the internal 100k pullup resisters (1 = enabled, 0 = disabled)
    void internalPullupMask(uint16_t mask);

    //Interrupt functionality (not yet implemented)


  private:
	void writeRegister(uint8_t regaddress, uint8_t val);
	void writeRegister(uint8_t regaddress, uint16_t val);
	uint16_t readRegister(uint8_t regaddress);

	//Our actual i2c address
	uint8_t _i2cAddress;
	I2cBus* _bus;

	//Cached copies of the register vales
	uint16_t _GPIO, _IODIR, _GPPU;
};
#endif 
