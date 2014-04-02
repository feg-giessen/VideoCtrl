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

ProjectorCtrl::ProjectorCtrl() {
    _status = _statusInvalid;
}

void ProjectorCtrl::begin(ip_addr_t addr, uint16_t port) {
	_client.begin(addr, port);
}

bool ProjectorCtrl::readStatus() {
    char* result;
    char cmd[6] = "CR0\r";
    size_t len;
    uint8_t statusCode, i;
    err_t error;

    error = _client.send(cmd, &len, &result);
    if (error != ERR_OK)
    	return false;

    if (result != NULL) {
        if (len >= 2) {
            statusCode = (uint8_t)atoi(result);

            for (i = 0; i < _statusCodesLength; i++) {
                if (_statusCodes[i] == statusCode) {
                    _status = i;
                    return true;
                }
            }
        }

        chHeapFree(result);
    }

    return false;
}

bool ProjectorCtrl::readTemperatures() {
    char* result;
    char cmd[6] = "CR6\r";
    size_t len;
    uint8_t i, part_length, tempp;

    if (_client.send(cmd, &len, &result) != ERR_OK)
    	return false;

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

        chHeapFree(result);
    }

    return tempp > 0;
}


uint8_t ProjectorCtrl::getStatus() {
    if (_status == _statusInvalid) {
        return 99;
    }

    return _statusCodes[_status];
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
    char* result;
    size_t len;

    if (value) cmd = (char*)"C00\r";
    else cmd = (char*)"C01\r";

    _client.send(cmd, &len, &result);

    if (result != NULL) {
        chHeapFree(result);
    }
}

void ProjectorCtrl::setVideoMute(bool value) {
    char* cmd;
    char* result;
    size_t len;

    if (value) cmd = (char*)"C0D\r";
    else cmd = (char*)"C0E\r";

    _client.send(cmd, &len, &result);

    if (result != NULL) {
        chHeapFree(result);
    }
}
