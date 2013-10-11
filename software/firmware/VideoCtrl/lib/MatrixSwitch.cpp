/*
 * MatrixSwitch.cpp
 *
 *  Created on: 08.09.2013
 *      Author: Peter Schuster
 */

#include "MatrixSwitch.h"
#include <string.h>
#include <stdio.h>

namespace chibios_rt {

MatrixSwitch::MatrixSwitch(ip_addr_t addr, uint16_t port) {
	_serial = new TcpSerialAdapter(addr, port);
	memset(_status, 0, 4);
}

void MatrixSwitch::setOutput(u8_t input, u8_t output){
	char* result;

	if (input < 1 || input > 4)
		return;

	if (output < 1 || output > 4)
		return;

	char cmd[13];
	sprintf(cmd, "sw i0%d o0%d\r\n", input, output);

	result = _serial->send(cmd);
	if (strncmp("Command OK", result, 10)) {
		_status[input] = output;
	}
}

unsigned char MatrixSwitch::getOutput(unsigned char input){
	return _status[input];
}

}
