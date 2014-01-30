/*
 * Memory.h
 *
 *  Created on: 06.11.2013
 *      Author: Peter Schuster
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include "../lib/hw/MCP24AA04.h"

#define MEM_ADDR_DATA_VERSION 	0x000
#define MEM_ADDR_IP_ADDRESS 	0x004
#define MEM_ADDR_MAPPING_BASE   0x100 // -> till 0x13F

typedef struct {
    uint8_t b1;
    uint8_t b2;
    uint8_t b3;
    uint8_t b4;

    uint8_t b5;
    uint8_t b6;
    uint8_t b7;
    uint8_t b8;

    uint8_t b9;
    uint8_t b10;
    uint8_t b11;
    uint8_t b12;

    uint8_t b13;
    uint8_t b14;
    uint8_t b15;
    uint8_t b16;
} button_mapping_t;

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
	msg_t getButtonMapping(uint8_t position, button_mapping_t* data);
	msg_t setButtonMapping(uint8_t position, button_mapping_t* data);

private:
	void migrate();
};

#endif /* MEMORY_H_ */
