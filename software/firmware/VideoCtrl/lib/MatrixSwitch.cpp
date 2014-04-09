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
	_serial.begin(addr, port, 10);
	memset(_status, 0, 4);
}

void MatrixSwitch::setInput(u8_t output, u8_t input){
	size_t len;

	if (input < 1 || input > 4)
		return;

	if (output < 1 || output > 4)
		return;

	char cmd[13];
	sprintf(cmd, "sw i0%d o0%d\r\n", input, output);
	len = strlen(cmd);

	matrix_msg_t* inout = new matrix_msg_t();
	inout->input = input;
	inout->output = output;

	if (_serial.send(cmd, &len, &_recv_cb, (void*)this, (void*)inout) != ERR_OK) {
	    delete inout;
	}

	return;
}

u8_t MatrixSwitch::getInput(u8_t output){
	return _status[output - 1];
}

bool MatrixSwitch::enableButtons(bool enabled) {
	size_t len;

	char* cmd_on = (char*)"button on\r\n";
	char* cmd_off = (char*)"button off\r\n";

	char* cmd = enabled ? cmd_on : cmd_off;

    if (_serial.send(cmd, &len, &_recv_cb, (void*)this, NULL) != ERR_OK) {
		return false;
    }

	return false;
}

void MatrixSwitch::_recv_cb(err_t err, void* context, char* result, size_t length, void* arg) {
    MatrixSwitch* that = (MatrixSwitch*)context;
    matrix_msg_t* inout = (matrix_msg_t*)arg;

    if (result != NULL) {
        if ((length >= (10 + 11) && strncmp("Command OK", result + 11, 10) == 0)
            || (length >= 10 && strncmp("Command OK", result, 10) == 0)) {

            if (inout->output <= 4 && inout->output >= 1
                    && inout->input <= 4 && inout->input >= 1) {

                that->_status[inout->output-1] = inout->input;
            }
        }
    }

    if (inout != NULL) {
        chHeapFree(inout);
    }
}
