/*
 * LgTv.cpp
 *
 *  Created on: 04.10.2014
 *      Author: Peter
 */

#include "LgTv.h"

LgTv::LgTv() {
    _power = false;
    _videoMute = false;
}

void LgTv::begin(ip_addr_t addr, uint16_t port) {
    _client.begin(addr, port, 10);
}

bool LgTv::getPower() {
    return _power;
}

void LgTv::readPower() {

    char cmd[9] = "ka 0 FF\r";
    size_t len = 7;

    _client.send(cmd, len, &_recv_cb, (void*)this, (void*)LGTV_CMD_PWR, 10);
}

void LgTv::setPower(bool value) {

    char cmd[9] = "ka 0 00\r";
    cmd[6] = value ? '1' : '0';
    size_t len = 7;

    _client.send(cmd, len, &_recv_cb, (void*)this, (void*)LGTV_CMD_PWR, 10);
}

bool LgTv::getVideoMute() {
    return _videoMute;
}

void LgTv::readVideoMute() {

    char cmd[9] = "kd 0 FF\r";
    size_t len = 7;

    _client.send(cmd, len, &_recv_cb, (void*)this, (void*)LGTV_CMD_PWR, 10);
}

void LgTv::setVideoMute(bool value) {

    char cmd[9] = "kd 0 00\r";
    cmd[6] = value ? '1' : '0';
    size_t len = 7;

    _client.send(cmd, len, &_recv_cb, (void*)this, (void*)LGTV_CMD_PWR, 10);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void LgTv::_recv_cb(err_t err, void* context, char* result, size_t length, void* arg) {
    LgTv* that = (LgTv*)context;
    uint32_t cmd = (uint32_t)arg;

    if (that == NULL)
        return;

    switch (cmd) {
    case LGTV_CMD_PWR:
        // Don't check error code here, we take what we can get (if length is sufficient).
        if (result != NULL) {
            if (length >= 6 && strncmp("a 0 OK00", result, 6) == 0) {
                that->_power = false;
            } else if (length >= 6 && strncmp("a 0 OK01", result, 6) == 0) {
                that->_power = true;
            }
        }
    case LGTV_CMD_VMU:
        // Don't check error code here, we take what we can get (if length is sufficient).
        if (result != NULL) {
            if (length >= 6 && strncmp("d 0 OK00", result, 6) == 0) {
                that->_videoMute = false;
            } else if (length >= 6 && strncmp("d 0 OK01", result, 6) == 0) {
                that->_videoMute = true;
            }
        }
        break;
    }
}

#pragma GCC diagnostic pop
