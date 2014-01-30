/*  PCA9685 LED library for Arduino
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
#include "PCA9685.h"

PCA9685::PCA9685() {}

void PCA9685::begin(I2cBus* bus, uint8_t i2cAddress) {
	_i2cAddress = PCA9685_I2C_BASE_ADDRESS | (i2cAddress & 0b00111111);
	_bus = bus;
}
bool PCA9685::init() {
    bool isOnline;

	writeRegister(PCA9685_MODE1, 0x01);	// reset the device

	uint8_t mode_reg;
	isOnline = (readRegister(PCA9685_MODE1, &mode_reg) == RDY_OK) && (mode_reg == 0x01);

	if (isOnline) {
        writeRegister(PCA9685_MODE1, 0b10100000);   // set up for auto increment
        writeRegister(PCA9685_MODE2, 0x10);	        // set to output
	}
	
	return isOnline;
}

void PCA9685::setLEDOn(uint8_t ledNumber) {
	writeLED(ledNumber, 0x1000, 0);
}

void PCA9685::setLEDOff(uint8_t ledNumber) {
	writeLED(ledNumber, 0, 0x1000);
}

void PCA9685::setLEDDimmed(uint8_t ledNumber, uint8_t amount) {		// Amount from 0-100 (off-on)
	if (amount==0)	{
		setLEDOff(ledNumber);
	} else if (amount>=100) {
		setLEDOn(ledNumber);
	} else {
		/*int randNumber = (int)random(4096);	// Randomize the phaseshift to distribute load. Good idea? Hope so. */
		// uint16_t randNumber = 1000;
	    // pseudo rand: ledNumber * 256 (max: 15 -> 15*256 = 3540)
	    uint16_t randNumber = ledNumber << 8;
		writeLED(ledNumber, randNumber, (uint16_t)(amount*41+randNumber) & 0xFFF);
	}
}

void PCA9685::writeLED(uint8_t ledNumber, uint16_t LED_ON, uint16_t LED_OFF) {	// LED_ON and LED_OFF are 12bit values (0-4095); ledNumber is 0-15
	if (ledNumber <= 15) {
		
		uint8_t buf[5];
		buf[0] = PCA9685_LED0 + 4*ledNumber;
		buf[1] = (uint8_t)(0xFF & LED_ON);
		buf[2] = (uint8_t)(0x1F & (LED_ON >> 8));
		buf[3] = (uint8_t)(0xFF & LED_OFF);
		buf[4] = (uint8_t)(0x1F & (LED_OFF >> 8));

		_bus->write(_i2cAddress, buf, sizeof(buf));
	}
}

//PRIVATE
msg_t PCA9685::writeRegister(uint8_t regAddress, uint8_t data) {
	uint8_t buf[] = { regAddress, data };

	return _bus->write(_i2cAddress, buf, sizeof(buf));
}

msg_t PCA9685::readRegister(uint8_t regAddress, uint8_t* val) {
	uint8_t buf[] = { regAddress };

	return _bus->read(_i2cAddress, buf, sizeof(buf), val, 1);
}
