/*
 * ProjectorCtrl.cpp
 *
 *  Created on: 18.11.2013
 *      Author: Peter Schuster
 */

#include "ProjectorCtrl.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

const uint8_t ProjectorCtrl::_statusCodes[] = { 0x00, 0x80, 0x40, 0x20, 0x10, 0x28, 0x24, 0x04, 0x21, 0x81, 0x88, 0x2C, 0x8C };
const char* ProjectorCtrl::_statusMessages[] = {
    (char*)"Power On",
    (char*)"Betriebsbereitschaft (Stand-By)",
    (char*)"Countdown",
    (char*)"Abkühlen",
    (char*)"Stromversorgungsfehler",
    (char*)"Unregelmäßigkeit beim Abkühlen",
    (char*)"Abkühlen zum Modus Automatische Lampenabschaltung",
    (char*)"Modus Automatische Lampenabschaltung nach dem Abkühlen",
    (char*)"Abkühlen nachdem der Projektor ausgeschaltet wurde wenn die Lampen aus sind.",
    (char*)"Betriebsbereitschaft nach dem Abkühlen wenn die Lampen ausgeschaltet sind.",
    (char*)"Betriebsbereitschaft nach dem Abkühlen wenn Unregelmäßigkeiten mit der Temperatur auftreten.",
    (char*)"Abkühlen nachdem der Projektor durch die Shutter-Steuerung ausgeschaltet wurde.",
    (char*)"Betriebsbereitschaft nach dem Abkühlen durch die Shutter-Steuerung"
};
const uint8_t ProjectorCtrl::_statusCodesLength = 13;
const uint8_t ProjectorCtrl::_statusInvalid = 99;

const char hex_lookup[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
const char hex_lookup2[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

ProjectorCtrl::ProjectorCtrl() {
    _status = _statusInvalid;
}

void ProjectorCtrl::begin(ip_addr_t addr, uint16_t port) {
	_client.begin(addr, port, 10);
}

void ProjectorCtrl::readStatus() {
    char cmd[6] = "CR0\r";
    size_t len = 4;

    _client.send(cmd, len, &_recv_cb, (void*)this, (void*)PROJECTOR_CMD_STAT, 3);
}

void ProjectorCtrl::readTemperatures() {
    char cmd[6] = "CR6\r";
    size_t len = 4;

    _client.send(cmd, len, &_recv_cb, (void*)this, (void*)PROJECTOR_CMD_TEMP, 15);
}


uint8_t ProjectorCtrl::getStatus() {
    if (_status == _statusInvalid) {
        return 99;
    }

    return _statusCodes[_status];
}

const char* ProjectorCtrl::getStatusMessage() {
    if (_status == _statusInvalid
            || _status >= _statusCodesLength) {
        return (char*)"Unbekannt";
    }

    return _statusMessages[_status];
}

char* ProjectorCtrl::getTemperature(uint8_t number) {
    if (number == 1) {
        return _temp1;
    } else if (number == 2) {
        return _temp2;
    } else if (number == 3) {
        return _temp3;
    }

    return NULL;
}

bool ProjectorCtrl::hasPower() {
    uint8_t status_code = getStatus();

    return status_code == 0x00      // power on
            || status_code == 0x40  // countdown
            || status_code == 0x20  // abkühlen
            || status_code == 0x24  // abkühlen (lampenabschaltung)
            || status_code == 0x21  // abkühlen (nach projektor aus lampe aus)
            || status_code == 0x2c; // abkühlen (shutter)
}

bool ProjectorCtrl::isPrePhase() {
    uint8_t status_code = getStatus();

    return status_code == 0x40;     // countdown
}

bool ProjectorCtrl::isPostPhase() {
    uint8_t status_code = getStatus();

    return status_code == 0x20      // abkühlen
            || status_code == 0x24  // abkühlen (lampenabschaltung)
            || status_code == 0x21  // abkühlen (nach projektor aus lampe aus)
            || status_code == 0x2c; // abkühlen (shutter)
}

bool ProjectorCtrl::isErrorStatus() {
    uint8_t status_code = getStatus();

    return status_code == 0x10      // stromversorgung
            || status_code == 0x28  // unregelm. abkühlen
            || status_code == 0x88  // unregelm. abkühlen
            || status_code == 99;   // invalid
}

void ProjectorCtrl::setPower(bool value) {
    char* cmd;
    size_t len;

    if (value) cmd = (char*)"C00\r";
    else cmd = (char*)"C01\r";

    len = 4;

    _client.send(cmd, len, &_recv_cb, (void*)this, (void*)PROJECTOR_CMD_NONE, 0);
}

void ProjectorCtrl::setVideoMute(bool value) {
    char* cmd;
    size_t len;

    if (value) cmd = (char*)"C0D\r";
    else cmd = (char*)"C0E\r";

    len = 4;

    _client.send(cmd, len, &_recv_cb, (void*)this, (void*)PROJECTOR_CMD_NONE, 0);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void ProjectorCtrl::_recv_cb(err_t err, void* context, char* result, size_t length, void* arg) {
    ProjectorCtrl* that = (ProjectorCtrl*)context;
    uint32_t cmd = (uint32_t)arg;

    if (that == NULL || result == NULL)
        return;

    switch (cmd) {
    case PROJECTOR_CMD_STAT:
        that->_parseStatus(result, length);
        break;
    case PROJECTOR_CMD_TEMP:
        that->_parseTemperatures(result, length);
        break;
    }
}

#pragma GCC diagnostic pop

void ProjectorCtrl::_parseStatus(char* result, size_t len) {
    uint8_t statusCode, i;

    if (result != NULL) {
        if (len >= 2) {
            statusCode = 0;
            for (i = 0; i < 16; i++) {
                if (result[0] == hex_lookup[i] || result[0] == hex_lookup2[i]) {
                    statusCode = (i << 4);
                    break;
                }
            }
            for (i = 0; i < 16; i++) {
                if (result[1] == hex_lookup[i] || result[1] == hex_lookup2[i]) {
                    statusCode |= i;
                    break;
                }
            }

            for (i = 0; i < _statusCodesLength; i++) {
                if (_statusCodes[i] == statusCode) {
                    _status = i;
                }
            }
        }
    }
}

void ProjectorCtrl::_parseTemperatures(char* result, size_t len) {
    uint8_t i, part_length, tempp;

    // reset pointer to temperature (0..2);
    tempp = 0;

    if (result != NULL) {
        part_length = 0;
        for (i = 0; i < len; i++) {

            // split on every space || end of string
            if (*(result + i) == ' ' || (i+1) == (uint8_t)len) {

                switch (tempp) {
                case 0:
                    if (part_length > sizeof(_temp1)) part_length = sizeof(_temp1);
                    memcpy(_temp1, (result + i - part_length), part_length);
                case 1:
                    if (part_length > sizeof(_temp2)) part_length = sizeof(_temp2);
                    memcpy(_temp2, (result + i - part_length), part_length);
                case 2:
                    if (part_length > sizeof(_temp3)) part_length = sizeof(_temp3);
                    memcpy(_temp3, (result + i - part_length), part_length);
                }

                part_length = 0;
                tempp += 1;
            } else {
                part_length += 1;
            }
        }
    }
}
