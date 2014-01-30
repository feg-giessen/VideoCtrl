/*
 * I2cBus.h
 *
 *  Created on: 12.10.2013
 *      Author: Peter Schuster
 */

#ifndef I2CBUS_H_
#define I2CBUS_H_

#include <stddef.h>
#include <stdint.h>
#include "chtypes.h"
#include "hal.h"

class I2cBus {
private:
	I2CDriver* _i2cp;
	systime_t _timeout;

public:
	I2cBus();
	void begin(I2CDriver* i2cp);
	msg_t write(i2caddr_t addr, const uint8_t *txbuf, size_t txbytes);
	msg_t read(i2caddr_t addr, const uint8_t *txbuf, size_t txbytes, uint8_t *rxbuf, size_t rxbytes);
};

#endif /* I2CBUS_H_ */
