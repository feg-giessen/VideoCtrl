/*
 * HdmiSwitch.cpp
 *
 *  Created on: 04.04.2014
 *      Author: Peter Schuster
 */

#include "HdmiSwitch.h"
#include <string.h>
#include <stdio.h>

HdmiSwitch::HdmiSwitch() {
}

void HdmiSwitch::begin(ip_addr_t addr, uint16_t port) {
    _serial.begin(addr, port);
    _status = 0;
}

void HdmiSwitch::setInput(u8_t input){
    char* result;
    size_t len;

    if (input < 1 || input > 4)
        return;

    char cmd[9];
    sprintf(cmd, "sw i0%d\r\n", input);

    if (_serial.send(cmd, &len, &result) != ERR_OK)
        return;

    if (result != NULL) {
        if (len >= 10 && strncmp("Command OK", result, 10)) {
            _status = input;
        }

        // free allocated memory.
        chHeapFree(result);
    }
}

u8_t HdmiSwitch::getInput(){
    return _status;
}
