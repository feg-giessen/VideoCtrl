/*
 * Memory.cpp
 *
 *  Created on: 06.11.2013
 *      Author: Peter Schuster
 */

#include "Memory.h"
#include "lwip/ip_addr.h"

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
