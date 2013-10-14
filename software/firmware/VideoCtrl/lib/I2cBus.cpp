/*
 * I2cBus.cpp
 *
 *  Created on: 12.10.2013
 *      Author: Peter Schuster
 */

#include "I2cBus.h"

#define IS_10BIT_MASK(addr) ((addr & ~0x3) == 0x78)

I2cBus::I2cBus(I2CDriver* i2cp) {
	_i2cp = i2cp;
	_timeout = MS2ST(4);
}

msg_t I2cBus::write(i2caddr_t addr, const uint8_t *txbuf, size_t txbytes) {
	if (IS_10BIT_MASK(addr)) {
		/* BI8 boards use invalid 11110xx addresses as 7-bit addresses. Workaround this... */
		return i2cMasterTransmitTimeout(
				_i2cp,
				((addr & 0x3) << 8 | *txbuf | (0xF0 << 8)),
				(uint8_t*)(((uint32_t)txbuf) + 1),
				txbytes-1,
				NULL,
				0,
				_timeout);
	} else {
		return i2cMasterTransmitTimeout(_i2cp, addr, txbuf, txbytes, NULL, 0, _timeout);
	}
}

msg_t I2cBus::read(i2caddr_t addr, const uint8_t *txbuf, size_t txbytes, uint8_t *rxbuf, size_t rxbytes) {
	if (IS_10BIT_MASK(addr)) {
		if (txbytes == 1) {
			return i2cMasterReceiveTimeout(
					_i2cp,
					((addr & 0x3) << 8 | *txbuf | (0xF0 << 8)),
					rxbuf,
					rxbytes,
					_timeout);
		} else {
			return i2cMasterTransmitTimeout(
					_i2cp,
					((addr & 0x3) << 8 | *txbuf | (0xF0 << 8)),
					(uint8_t*)(((uint32_t)txbuf) + 1),
					txbytes-1,
					rxbuf,
					rxbytes,
					_timeout);
		}
	} else {
		return i2cMasterTransmitTimeout(_i2cp, addr, txbuf, txbytes, rxbuf, rxbytes, _timeout);
	}
}
