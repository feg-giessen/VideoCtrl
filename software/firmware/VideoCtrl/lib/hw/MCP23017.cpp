/*  MCP23017 library for Arduino
    Copyright (C) 2009 David Pye    <davidmpye@gmail.com  
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

#include "MCP23017.h"

// IMPORTANT on byte order:
// Note on Writing words: The MSB is for GPA7-0 and the LSB is for GPB7-0
// Pinnumbers 0-7 = GPB0-7, 8-15 = GPA0-7
// This comes across as slightly un-intuitive when programming

MCP23017::MCP23017() {}

void MCP23017::begin(I2cBus* bus, uint8_t i2cAddress) {
	_i2cAddress = (MCP23017_I2C_BASE_ADDRESS >>1) | (i2cAddress & 0x07);
	_bus = bus;

	//Default state is 0 for our pins
	_GPIO = 0x0000;
	_IODIR = 0xFFFF;
	_GPPU = 0x0000;
}

bool MCP23017::init()	{
		// If this value is true (return value of this function), we assume the board actually responded and is "online"
    uint16_t status_val;
	if (readRegister(0x00, &status_val) != RDY_OK)
	    return false;
	
	//Set the IOCON.BANK bit to 0 to enable sequential addressing
	//IOCON 'default' address is 0x05, but will
	//change to our definition of IOCON once this write completes.
	writeRegister(0x05, (uint8_t)0x0);
	
	//Our pins default to being outputs by default.
	writeRegister(MCP23017_IODIR, _IODIR);
	
	return true;
}

uint16_t MCP23017::digitalWordRead() {
	readRegister(MCP23017_GPIO, &_GPIO);
	return _GPIO;
}
void MCP23017::digitalWordWrite(uint16_t w) {
	_GPIO = w;
	writeRegister(MCP23017_GPIO, _GPIO);
}

void MCP23017::inputPolarityMask(uint16_t mask) {
	writeRegister(MCP23017_IPOL, mask);
}

void MCP23017::inputOutputMask(uint16_t mask) {
	_IODIR = mask;
	writeRegister(MCP23017_IODIR, _IODIR);
}

void MCP23017::internalPullupMask(uint16_t mask) {
	_GPPU = mask;
	writeRegister(MCP23017_GPPU, _GPPU);
}

//PRIVATE
msg_t MCP23017::writeRegister(uint8_t regAddress, uint8_t data) {
	uint8_t buf[] = { regAddress, data };

	return _bus->write(_i2cAddress, buf, sizeof(buf));
}

msg_t MCP23017::writeRegister(uint8_t regAddress, uint16_t data) {
    uint8_t buf[3];
	buf[0] = regAddress;
	buf[1] = (uint8_t)(0xFF & data);
	buf[2] = (uint8_t)(0xFF & (data >> 8));

	return _bus->write(_i2cAddress, buf, sizeof(buf));
}

msg_t MCP23017::readRegister(uint8_t regAddress, uint16_t* value) {
    msg_t result;
    uint8_t rxbuf[2];
    uint8_t buf[] = { regAddress };
	rxbuf[0] = 0;
	rxbuf[1] = 0;

	result = _bus->read(_i2cAddress, buf, sizeof(buf), rxbuf, sizeof(rxbuf));
    
	*value = (uint16_t)(rxbuf[1] << 8 | (rxbuf[0]));

	return result;
} 
