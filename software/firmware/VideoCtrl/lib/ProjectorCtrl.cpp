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
    (char*)"Abk�hlen",
    (char*)"Stromversorgungsfehler",
    (char*)"Unregelm��igkeit beim Abk�hlen",
    (char*)"Abk�hlen zum Modus Automatische Lampenabschaltung",
    (char*)"Modus Automatische Lampenabschaltung nach dem Abk�hlen",
    (char*)"Abk�hlen nachdem der Projektor ausgeschaltet wurde wenn die Lampen aus sind.",
    (char*)"Betriebsbereitschaft nach dem Abk�hlen wenn die Lampen ausgeschaltet sind.",
    (char*)"Betriebsbereitschaft nach dem Abk�hlen wenn Unregelm��igkeiten mit der Temperatur auftreten.",
    (char*)"Abk�hlen nachdem der Projektor durch die Shutter-Steuerung ausgeschaltet wurde.",
    (char*)"Betriebsbereitschaft nach dem Abk�hlen durch die Shutter-Steuerung"
};
const uint8_t _statusCodesLength = 13;
const uint8_t _statusInvalid = 99;

ProjectorCtrl::ProjectorCtrl(ip_addr_t addr, uint16_t port) {
    _client = new TcpSerialAdapter(addr, port);
}

bool ProjectorCtrl::readStatus() {
    char* result;
    char cmd[6] = "CR0\r\n";
    size_t len;
    uint8_t statusCode, i;

    result = _client->send(cmd, &len);

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
    char cmd[6] = "CR6\r\n";
    size_t len;
    uint8_t i, part_length, tempp;

    result = _client->send(cmd, &len);

    // resest pointer to temperature (0..2);
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


const char* ProjectorCtrl::getStatus() {
    if (_status == _statusInvalid) {
        return (char*)"<Unbekannt>";
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

void ProjectorCtrl::setPower(bool value) {
    char* cmd;
    char* result;
    size_t len;

    if (value) cmd = (char*)"C00\r\n";
    else cmd = (char*)"C01\r\n";

    result = _client->send(cmd, &len);

    if (result != NULL) {
        chHeapFree(result);
    }
}

void ProjectorCtrl::setVideoMute(bool value) {
    char* cmd;
    char* result;
    size_t len;

    if (value) cmd = (char*)"C0D\r\n";
    else cmd = (char*)"C0E\r\n";

    result = _client->send(cmd, &len);

    if (result != NULL) {
        chHeapFree(result);
    }
}