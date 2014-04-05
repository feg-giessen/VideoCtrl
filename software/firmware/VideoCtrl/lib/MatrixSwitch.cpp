/*
 * MatrixSwitch.cpp
 *
 *  Created on: 08.09.2013
 *      Author: Peter Schuster
 */

#include "MatrixSwitch.h"
#include <string.h>
#include <stdio.h>

MatrixSwitch::MatrixSwitch() {
}

void MatrixSwitch::begin(ip_addr_t addr, uint16_t port) {
	_serial.begin(addr, port);
	memset(_status, 0, 4);
}

void MatrixSwitch::setInput(u8_t output, u8_t input){
	char* result;
	size_t len;

	if (input < 1 || input > 4)
		return;

	if (output < 1 || output > 4)
		return;

	char cmd[13];
	sprintf(cmd, "sw i0%d o0%d\r\n", input, output);

	if (_serial.send(cmd, &len, &result) != ERR_OK)
		return;

	if (result != NULL) {
	    if (len >= 10 && strncmp("Command OK", result, 10) == 0) {
            _status[output - 1] = input;
        }

	    // free allocated memory.
	    chHeapFree(result);
	}
}

u8_t MatrixSwitch::getInput(u8_t output){
	return _status[output - 1];
}

bool MatrixSwitch::enableButtons(bool enabled) {
	char* result;
	size_t len;

	char* cmd_on = (char*)"button on\r\n";
	char* cmd_off = (char*)"button off\r\n";

	char* cmd = enabled ? cmd_on : cmd_off;

	if (_serial.send(cmd, &len, &result) != ERR_OK)
		return false;

	if (result != NULL) {
	    if (len >= 10 && strncmp("Command OK", result, 10) == 0) {
	    	// free allocated memory.
	    	chHeapFree(result);
	    	return true;
        }

	    // free allocated memory.
	    chHeapFree(result);
	}

	return false;
}
