/*
 * Memory.cpp
 *
 *  Created on: 06.11.2013
 *      Author: Peter Schuster
 */

#include "Memory.h"
#include "lwip/ip_addr.h"
#include "Videomischer.h"

Memory::Memory() {
	_ip_address = 0;
}

void Memory::init(MCP24AA04* eeprom) {
	_eeprom = eeprom;
}

void Memory::migrate() {
	uint8_t dversion = getDataVersion();

	// Migration to v1
	if (dversion <= 0) {
		_dversion = 1;
		_eeprom->write(MEM_ADDR_DATA_VERSION, &_dversion, 1);

		ip_addr_t ip;
		IP4_ADDR(&ip, 192, 168, 40, 20);

		setIpAddress(ip.addr);

		button_mapping_t default_mapping;
		default_mapping.b1  = (uint8_t)ATEM_Program;
		default_mapping.b2  = (uint8_t)ATEM_Preview;
		default_mapping.b3  = (uint8_t)ATEM_Aux1;
		default_mapping.b4  = (uint8_t)ATEM_None;
		default_mapping.b5  = (uint8_t)ATEM_None;
		default_mapping.b6  = (uint8_t)ATEM_Dsk1;
		default_mapping.b7  = (uint8_t)ATEM_Macro1;
		default_mapping.b8  = (uint8_t)ATEM_FadeToBlack;

		default_mapping.b9  = (uint8_t)ATEM_In1;
		default_mapping.b10 = (uint8_t)ATEM_In2;
		default_mapping.b11 = (uint8_t)ATEM_In3;
		default_mapping.b12 = (uint8_t)ATEM_In4;
		default_mapping.b13 = (uint8_t)ATEM_In5;
		default_mapping.b14 = (uint8_t)ATEM_In6;
		default_mapping.b15 = (uint8_t)ATEM_None;
		default_mapping.b16 = (uint8_t)ATEM_InBlack;

		setButtonMapping(0, &default_mapping);
	}
}

uint32_t Memory::getIpAddress() {
	if (_ip_address == 0) {
		uint8_t data[4];
		_eeprom->read(MEM_ADDR_IP_ADDRESS, data, sizeof(data));

		_ip_address = ((uint32_t)data[0])
				| (((uint32_t)data[1]) << 8)
				| (((uint32_t)data[2]) << 16)
				| (((uint32_t)data[3]) << 24);
	}

	return _ip_address;
}

void Memory::setIpAddress(uint32_t ip_address) {
	uint8_t data[] = {
		(uint8_t)(0xFF & (ip_address >> 0)),
		(uint8_t)(0xFF & (ip_address >> 8)),
		(uint8_t)(0xFF & (ip_address >> 16)),
		(uint8_t)(0xFF & (ip_address >> 24))
	};
	_eeprom->write(MEM_ADDR_IP_ADDRESS, data, sizeof(data));
	_ip_address = ip_address;
}

uint8_t Memory::getDataVersion() {
	if (_dversion == 0) {
		uint8_t data[1];
		_eeprom->read(MEM_ADDR_DATA_VERSION, data, sizeof(data));

		_dversion = data[0];
	}

	return _dversion;
}

msg_t Memory::getButtonMapping(uint8_t position, button_mapping_t* data) {
    return _eeprom->read(
            MEM_ADDR_MAPPING_BASE + (position * sizeof(button_mapping_t)),
            (uint8_t*)data,
            sizeof(button_mapping_t));
}

msg_t Memory::setButtonMapping(uint8_t position, button_mapping_t* data) {
    return _eeprom->write(
            MEM_ADDR_MAPPING_BASE + (position * sizeof(button_mapping_t)),
            (uint8_t*)data,
            sizeof(button_mapping_t));
}
