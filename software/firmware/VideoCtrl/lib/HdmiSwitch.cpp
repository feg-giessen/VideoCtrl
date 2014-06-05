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
    _client.begin(addr, port, 10);
    _status = 0;
}

u8_t HdmiSwitch::getInput(){
    return _status;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"

void HdmiSwitch::setInput(u8_t input){
    size_t len;

    if (input < 1 || input > 4)
        return;

    char cmd[9];
    sprintf(cmd, "sw i0%d\r\n", input);
    len = 9; // strlen(cmd); <-- fixed length for input = [1..4]

    _client.send(cmd, len, &_recv_cb, (void*)this, (void*)input, 23);
}

bool HdmiSwitch::enableButtons(bool enabled) {
    size_t len;

    char* cmd_on = (char*)"button on\r\n";
    char* cmd_off = (char*)"button off\r\n";

    char* cmd = enabled ? cmd_on : cmd_off;
    len = enabled ? 11 : 12;

    if (_client.send(cmd, len, &_recv_cb, (void*)this, NULL, 23) != ERR_OK) {
        return false;
    }

    return true;
}

#pragma GCC diagnostic ignored "-Wunused-parameter"

void HdmiSwitch::_recv_cb(err_t err, void* context, char* result, size_t length, void* arg) {
    HdmiSwitch* that = (HdmiSwitch*)context;
    u32_t in = (u32_t)arg;

    // Don't check error code here, we take what we can get (if length is sufficient).
    if (result != NULL) {
        if ((length >= (10 + 11) && strncmp("Command OK", result + 11, 10) == 0)
            || (length >= 10 && strncmp("Command OK", result, 10) == 0)) {

            if (in <= 4 && in >= 1) {
                that->_status = in;
            }
        }
    }
}

#pragma GCC diagnostic pop
