/*
 * I2cBus.cpp
 *
 *  Created on: 12.10.2013
 *      Author: Peter Schuster
 */

#include "I2cBus.h"

I2cBus::I2cBus(I2CDriver* i2cp) {
	_i2cp = i2cp;
	_timeout = MS2ST(4);
}

msg_t I2cBus::write(i2caddr_t addr, const uint8_t *txbuf, size_t txbytes) {
	return i2cMasterTransmitTimeout(_i2cp, addr, txbuf, txbytes, NULL, 0, _timeout);
}

msg_t I2cBus::read(i2caddr_t addr, const uint8_t *txbuf, size_t txbytes, uint8_t *rxbuf, size_t rxbytes) {
	return i2cMasterTransmitTimeout(_i2cp, addr, txbuf, txbytes, rxbuf, rxbytes, _timeout);
}
