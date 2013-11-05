/*
 * MCP24AA04.cpp
 *
 *  Created on: 02.11.2013
 *      Author: Peter Schuster
 */

#include "ch.h"
#include "MCP24AA04.h"
#include <string.h>

#define PINC(p, n) (uint8_t*)(((uint32_t)p) + n)
#define GETADDR(a) MCP24AA04_I2C_BASE_ADDRESS | ((a >> 8) & 0x1)

MCP24AA04::MCP24AA04() {
}

void MCP24AA04::begin(I2cBus* bus) {
	_bus = bus;
}

msg_t MCP24AA04::write(uint16_t addr, const uint8_t* data, size_t count) {
    chDbgAssert((addr >> 9) == 0,
                "MCP24AA04::write, #1",
                "Invalid data address (more than 9 bits)");

    // local data array to include address
    uint8_t* ldata = (uint8_t*)chHeapAlloc(NULL, count + 1);

    memcpy(PINC(ldata, 1), data, count);
    *ldata = 0xFF & addr;

    msg_t result;
    result = _bus->write(GETADDR(addr), ldata, count + 1);

    chHeapFree(ldata);

    return result;
}

msg_t MCP24AA04::read(uint16_t addr, uint8_t* data, size_t count) {
    chDbgAssert((addr >> 9) == 0,
                "MCP24AA04::write, #1",
                "Invalid data address (more than 9 bits)");

    uint8_t laddr = 0xFF & addr;

    return _bus->read(GETADDR(addr), &laddr, 1, data, count);
}
