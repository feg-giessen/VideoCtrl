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
    _query_in_progress = false;
    _set_power_in_progress = false;
    _set_videoMute_in_progress = false;
    _power_read  = false;
    _videoMute_read = false;
}

void LgTv::begin(ip_addr_t addr, uint16_t port) {
    _client.begin(addr, port, 10);
    _power_read  = false;
    _videoMute_read = false;
}

bool LgTv::getPower() {
    return _power;
}

bool LgTv::isPowerInitialized() {
    return _power_read;
}

void LgTv::readPower() {
    if (_query_in_progress)
        return;

    char cmd[10] = "ka 00 FF\r";
    size_t len = 9;

    _client.send(cmd, len, &_recv_cb, (void*)this, (void*)LGTV_CMD_PWR, 10);
    _query_in_progress = true;
}

void LgTv::setPower(bool value) {
    if (_set_power_in_progress)
        return;

    char cmd[10] = "ka 00 00\r";
    cmd[7] = value ? '1' : '0';
    size_t len = 9;

    _client.send(cmd, len, &_recv_cb, (void*)this, (void*)LGTV_CMD_PWR, 10);
    _set_power_in_progress = true;
}

bool LgTv::getVideoMute() {
    return _videoMute;
}

bool LgTv::isVideoMuteInitialized() {
    return _videoMute_read;
}

void LgTv::readVideoMute() {
    if (_query_in_progress)
        return;

    char cmd[10] = "kd 00 FF\r";
    size_t len = 9;

    _client.send(cmd, len, &_recv_cb, (void*)this, (void*)LGTV_CMD_VMU, 10);
    _query_in_progress = true;
}

void LgTv::setVideoMute(bool value) {
    if (_set_videoMute_in_progress)
        return;

    char cmd[10] = "kd 00 00\r";
    cmd[7] = value ? '1' : '0';
    size_t len = 9;

    _client.send(cmd, len, &_recv_cb, (void*)this, (void*)LGTV_CMD_VMU, 10);
    _set_videoMute_in_progress = true;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void LgTv::_recv_cb(err_t err, void* context, char* result, size_t length, void* arg) {
    LgTv* that = (LgTv*)context;
    uint32_t cmd = (uint32_t)arg;

    if (that == NULL)
        return;

    that->_query_in_progress = false;

    switch (cmd) {
    case LGTV_CMD_PWR:
        // Don't check error code here, we take what we can get (if length is sufficient).
        if (result != NULL) {
            that->_power_read = true;
            if (length >= 9 && strncmp("a ", result, 2) == 0 && strncmp("OK00", result + 5, 4) == 0) {
                that->_power = false;
            } else if (length >= 9 && strncmp("a ", result, 2) == 0 && strncmp("OK01", result + 5, 4) == 0) {
                that->_power = true;
            } else {
                that->_power_read = false;
            }
        }
        that->_set_power_in_progress = false;
        break;
    case LGTV_CMD_VMU:
        // Don't check error code here, we take what we can get (if length is sufficient).
        if (result != NULL) {
            that->_videoMute_read = true;
            if (length >= 9 && strncmp("d ", result, 2) == 0 && strncmp("OK00", result + 5, 4) == 0) {
                that->_videoMute = false;
            } else if (length >= 9 && strncmp("d ", result, 2) == 0 && strncmp("OK01", result + 5, 4) == 0) {
                that->_videoMute = true;
            } else {
                that->_videoMute_read = false;
            }
        }
        that->_set_videoMute_in_progress = false;
        break;
    }
}

#pragma GCC diagnostic pop
