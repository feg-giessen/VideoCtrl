/*
 * MCP24AA04.h
 *
 * EEPROM chip
 *
 *  Created on: 02.11.2013
 *      Author: Peter Schuster
 */

#ifndef MCP24AA04_H_
#define MCP24AA04_H_

#include "../I2cBus.h"

#define MCP24AA04_I2C_BASE_ADDRESS 0b1010000

class MCP24AA04 {
private:
	I2cBus* _bus;
public:
	MCP24AA04();
    void begin(I2cBus* bus);
    msg_t write(uint16_t addr, const uint8_t* data, size_t count);
    msg_t read(uint16_t addr, uint8_t* data, size_t count);
};

#endif /* MCP24AA04_H_ */
