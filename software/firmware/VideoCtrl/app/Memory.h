/*
 * Memory.h
 *
 *  Created on: 06.11.2013
 *      Author: Peter Schuster
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include "../lib/hw/MCP24AA04.h"

#define MEM_ADDR_DATA_VERSION 	0x00
#define MEM_ADDR_IP_ADDRESS 	0x04

class Memory {
private:
	MCP24AA04* _eeprom;

	uint8_t _dversion;
	uint32_t _ip_address;
public:
	Memory();
	void init(MCP24AA04* eeprom);

	uint32_t getIpAddress();
	void setIpAddress(uint32_t ip_address);
	uint8_t getDataVersion();

private:
	void migrate();
};

#endif /* MEMORY_H_ */
