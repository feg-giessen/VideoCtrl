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
    (char*)"720P",      // 8
    (char*)"1080I",     // 9
    (char*)"1080P",     // 10
    (char*)"576I",      // 11
    (char*)"576P",      // 12
    (char*)"720P",      // 13
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
    _client.begin(addr, port);
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
    char* result;
    size_t len;

    char* cmd = (char*)"R POWER\r\n";

    if (_client.send(cmd, &len, &result) != ERR_OK || result == NULL) {
        _power = false;
        return;
    }

    _power = (len >= 10 && strncmp("> POWER ON", result, 10));
    chHeapFree(result);
}

void TvOneScaler::readSource() {
    char* result;
    size_t len;

    char* cmd = (char*)"R SOURCE\r\n";

    if (_client.send(cmd, &len, &result) != ERR_OK || result == NULL) {
        _source = -1;
        return;
    }

    // result: "> SOURCE HDMI"
    //                   ^
    //          0123456789

    uint8_t i, j;
    for (i = 0; i < SOURCES_COUNT; i++) {
        for (j = 0; j <= (len - 9); j++) {
            if (_sources[i][j] == 0) {
                if (j > 0) {
                    // end of string -> found it;
                    chHeapFree(result);
                    _source = i;
                    return;
                } else {
                    break;
                }
            } else if (_sources[i][j] != result[9 + j]) {
                // mismatch -> try next.
                break;
            }
        }

        if (j == (len - 9)) {
            // iterated through all chars -> found it;
            chHeapFree(result);
            _source = i;
            return;
        }
    }

    chHeapFree(result);
    _source = -1;
    return;
}

void TvOneScaler::readOutput() {
    char* result;
    size_t len;

    char* cmd = (char*)"R OUTPUT\r\n";

    if (_client.send(cmd, &len, &result) != ERR_OK || result == NULL) {
        _output = -1;
        return;
    }

    // result: "> OUTPUT 1080I50"
    //                   ^
    //          0123456789

    uint8_t i, j;
    for (i = 0; i < OUTPUTS_COUNT; i++) {
        for (j = 0; j <= (len - 9); j++) {
            if (_outputs[i][j] == 0) {
                if (j > 0) {
                    // end of string -> found it;
                    chHeapFree(result);
                    _output = i;
                    return;
                } else {
                    break;
                }
            } else if (_outputs[i][j] != result[9 + j]) {
                // mismatch -> try next.
                break;
            }
        }

        if (j == (len - 9)) {
            // iterated through all chars -> found it;
            chHeapFree(result);
            _output = i;
            return;
        }
    }

    chHeapFree(result);
    _output = -1;
    return;
}

void TvOneScaler::readSize() {
    char* result;
    size_t len;

    char* cmd = (char*)"R SIZE\r\n";

    if (_client.send(cmd, &len, &result) != ERR_OK || result == NULL) {
        _size = -1;
        return;
    }

    // result: "> SIZE FULL"
    //                 ^
    //          01234567

    uint8_t i, j;
    for (i = 0; i < SIZES_COUNT; i++) {
        for (j = 0; j <= (len - 7); j++) {
            if (_sizes[i][j] == 0) {
                if (j > 0) {
                    // end of string -> found it;
                    chHeapFree(result);
                    _size = i;
                    return;
                } else {
                    break;
                }
            } else if (_sizes[i][j] != result[7 + j]) {
                // mismatch -> try next.
                break;
            }
        }

        if (j == (len - 7)) {
            // iterated through all chars -> found it;
            chHeapFree(result);
            _size = i;
            return;
        }
    }

    chHeapFree(result);
    _size = -1;
    return;
}

void TvOneScaler::setPower(bool power) {
    char* result;
    size_t len;

    char* cmd;

    if (power) {
        cmd = (char*)"S POWER ON\r\n";
        len = 12;
    } else {
        cmd = (char*)"S POWER OFF\r\n";
        len = 13;
    }

    if (_client.send(cmd, &len, &result) != ERR_OK)
        return;

    if (result != NULL) {
        // free allocated memory.
        chHeapFree(result);
    }
}

void TvOneScaler::setSource(uint8_t value) {
    char* result;
    size_t len;

    char cmd[13];

    if (value >= SOURCES_COUNT)
        return;

    sprintf(cmd, "S SOURCE %d\r\n", value);

    if (_client.send(cmd, &len, &result) != ERR_OK)
        return;

    if (result != NULL) {
        // free allocated memory.
        chHeapFree(result);
    }
}

void TvOneScaler::setOutput(uint8_t value) {
    char* result;
    size_t len;

    char cmd[14];

    if (value >= OUTPUTS_COUNT)
        return;

    sprintf(cmd, "S OUTPUT %d\r\n", value);

    if (_client.send(cmd, &len, &result) != ERR_OK)
        return;

    if (result != NULL) {
        // free allocated memory.
        chHeapFree(result);
    }
}

void TvOneScaler::setSize(uint8_t value) {
    char* result;
    size_t len;

    char cmd[11];

    if (value >= SIZES_COUNT)
        return;

    sprintf(cmd, "S SIZE %d\r\n", value);

    if (_client.send(cmd, &len, &result) != ERR_OK)
        return;

    if (result != NULL) {
        // free allocated memory.
        chHeapFree(result);
    }
}
