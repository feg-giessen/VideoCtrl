/*
 * TvOneScaler.cpp
 *
 *  Created on: 04.04.2014
 *      Author: Peter Schuster
 */

#include "TvOneScaler.h"
#include <string.h>
#include <stdio.h>

const char* TvOneScaler::_sources[] = {
    (char*)"CV",    // 0
    (char*)"SV",    // 1
    (char*)"COMP",  // 2
    (char*)"PC",    // 3
    (char*)"HDMI"   // 4
};

const char* TvOneScaler::_outputs[] = {
    (char*)"NATIVE",    // 0
    (char*)"VGA",       // 1
    (char*)"SVGA",      // 2
    (char*)"XGA",       // 3
    (char*)"SXGA",      // 4
    (char*)"UXGA",      // 5
    (char*)"480I",      // 6
    (char*)"480P",      // 7
    (char*)"720P",      // 8  (60)
    (char*)"1080I",     // 9
    (char*)"1080P",     // 10
    (char*)"576I",      // 11
    (char*)"576P",      // 12
    (char*)"720P",      // 13 (50)
    (char*)"1080I50",   // 14
    (char*)"1080P50",   // 15
    (char*)"WXGA",      // 16
    (char*)"WSXGA",     // 17
    (char*)"WUXGA",     // 18
    (char*)"WXGA+"      // 19
};

const char* TvOneScaler::_sizes[] = {
    (char*)"FULL",      // 0
    (char*)"OVERSCAN",  // 1
    (char*)"UNDERSCAN", // 2
    (char*)"LETTERBOX", // 3
    (char*)"PANSCAN"    // 4
};

TvOneScaler::TvOneScaler() {
    _power = false;
    _source = -1;
    _output = -1;
    _size = -1;
}

void TvOneScaler::begin(ip_addr_t addr, uint16_t port) {
    _client.begin(addr, port, 10);
}

bool TvOneScaler::getPower() {
    return _power;
}

int8_t TvOneScaler::getSource() {
    return _source;
}

int8_t TvOneScaler::getOutput() {
    return _output;
}

int8_t TvOneScaler::getSize() {
    return _size;
}

void TvOneScaler::readPower() {
    size_t len;

    char* cmd = (char*)"R POWER\r\n";
    len = strlen(cmd);

    _client.send(cmd, len, &_recv_cb, (void*)this, (void*)SCALER_CMD_PWR, 12);
}

void TvOneScaler::readSource() {
    size_t len;

    char* cmd = (char*)"R SOURCE\r\n";
    len = strlen(cmd);

    _client.send(cmd, len, &_recv_cb, (void*)this, (void*)SCALER_CMD_SRCS, 14);
}

void TvOneScaler::readOutput() {
    size_t len;

    char* cmd = (char*)"R OUTPUT\r\n";
    len = strlen(cmd);

    _client.send(cmd, len, &_recv_cb, (void*)this, (void*)SCALER_CMD_OUT, 17);
}

void TvOneScaler::readSize() {
    size_t len;

    char* cmd = (char*)"R SIZE\r\n";
    len = strlen(cmd);

    _client.send(cmd, len, &_recv_cb, (void*)this, (void*)SCALER_CMD_SIZE, 17);
}

void TvOneScaler::setPower(bool power) {
    size_t len;

    char* cmd;

    if (power) {
        cmd = (char*)"S POWER ON\r\n";
        len = 12;
    } else {
        cmd = (char*)"S POWER OFF\r\n";
        len = 13;
    }

    _client.send(cmd, len, &_recv_cb, (void*)this, (void*)SCALER_CMD_NONE, -1);
}

void TvOneScaler::setSource(uint8_t value) {
    size_t len;

    char cmd[13];

    if (value >= SOURCES_COUNT)
        return;

    sprintf(cmd, "S SOURCE %d\r\n", value);
    len = strlen(cmd);

    _client.send(cmd, len, &_recv_cb, (void*)this, (void*)SCALER_CMD_NONE, -1);
}

void TvOneScaler::setOutput(uint8_t value) {
    size_t len;

    char cmd[14];

    if (value >= OUTPUTS_COUNT)
        return;

    sprintf(cmd, "S OUTPUT %d\r\n", value);
    len = strlen(cmd);

    _client.send(cmd, len, &_recv_cb, (void*)this, (void*)SCALER_CMD_NONE, -1);
}

void TvOneScaler::setSize(uint8_t value) {
    size_t len;

    char cmd[11];

    if (value >= SIZES_COUNT)
        return;

    sprintf(cmd, "S SIZE %d\r\n", value);
    len = strlen(cmd);

    _client.send(cmd, len, &_recv_cb, (void*)this, (void*)SCALER_CMD_NONE, -1);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void TvOneScaler::_recv_cb(err_t err, void* context, char* result, size_t length, void* arg) {
    TvOneScaler* that = (TvOneScaler*)context;
    uint32_t cmd = (uint32_t)arg;

    if (that == NULL || result == NULL)
        return;

    switch (cmd) {
    case SCALER_CMD_PWR:
        that->_parsePower(result, length);
        break;
    case SCALER_CMD_SRCS:
        that->_parseSource(result, length);
        break;
    case SCALER_CMD_OUT:
        that->_parseOutput(result, length);
        break;
    case SCALER_CMD_SIZE:
        that->_parseSize(result, length);
        break;
    }
}

#pragma GCC diagnostic pop

void TvOneScaler::_parsePower(char* result, size_t len) {

    if (result == NULL || len == 0)
        return;

    _power = (len >= 12 && strncmp("> POWER ON", result + 2, 10) == 0); // +2 -> \r\n at start of response
}

void TvOneScaler::_parseSource(char* result, size_t len) {

    if (result == NULL || len == 0)
        return;

    // result: "\r\n> SOURCE HDMI"
    //                       ^
    //          0 1 2345678..11

    uint8_t i, j;
    for (i = 0; i < SOURCES_COUNT; i++) {
        for (j = 0; j <= (len - 11); j++) {
            if (_sources[i][j] == 0) {
                if (j > 0) {
                    // end of string -> found it;
                    _source = i;
                    return;
                } else {
                    break;
                }
            } else if (_sources[i][j] != result[11 + j]) {
                // mismatch -> try next.
                break;
            }
        }

        if (j == (len - 11)) {
            // iterated through all chars -> found it;
            _source = i;
            return;
        }
    }

    _source = -1;
}

void TvOneScaler::_parseOutput(char* result, size_t len) {

    if (result == NULL || len == 0)
        return;

    // result: "\r\n> OUTPUT 1080I50"
    //                       ^
    //          0 1 2345678..11

    int i;
    uint8_t j;
    // reversed walk: longer strings are at end & prefer 50 Hz.
    for (i = (OUTPUTS_COUNT - 1); i >= 0; i--) {
        for (j = 0; j <= (len - 11); j++) {
            if (_outputs[i][j] == 0) {
                if (j > 0) {
                    // end of string -> found it;
                    _output = i;
                    return;
                } else {
                    break;
                }
            } else if (_outputs[i][j] != result[11 + j]) {
                // mismatch -> try next.
                break;
            }
        }

        if (j == (len - 11)) {
            // iterated through all chars -> found it;
            _output = i;
            return;
        }
    }

    _output = -1;
}

void TvOneScaler::_parseSize(char* result, size_t len) {

    if (result == NULL || len == 0)
        return;

    // result: "\r\n> SIZE FULL"
    //                     ^
    //          0 1 23456789

    uint8_t i, j;
    for (i = 0; i < SIZES_COUNT; i++) {
        for (j = 0; j <= (len - 9); j++) {
            if (_sizes[i][j] == 0) {
                if (j > 0) {
                    // end of string -> found it;
                    _size = i;
                    return;
                } else {
                    break;
                }
            } else if (_sizes[i][j] != result[9 + j]) {
                // mismatch -> try next.
                break;
            }
        }

        if (j == (len - 9)) {
            // iterated through all chars -> found it;
            _size = i;
            return;
        }
    }

    _size = -1;
}

