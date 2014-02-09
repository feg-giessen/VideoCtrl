/*
 * ViscaController.cpp
 *
 *  Created on: 03.02.2014
 *      Author: Peter Schuster
 */

#include "ViscaController.h"

ViscaController::ViscaController() {
    _sdp = NULL;
    _state = ViscaState_Initial;
    _cam_addr = 0;
}

void ViscaController::begin(SerialDriver* sdp) {
    uint8_t i;

    _sdp = sdp;
    _state = ViscaState_Initial;

    for (i = 0; i < VISCA_SOCKET_COUNT; i++) {
        _sockets[i] = ViscaSocketState_Empty;
    }
}

void ViscaController::processPackets() {
    int8_t msg = 0;

    while ((msg = getPacket()) != -1) {

        // handle overflow
        if (msg == -2) {
            _clearIQ();
        }
    }
}

int8_t ViscaController::getPacket() {
    msg_t m;
    size_t index;
    uint8_t b[VISCA_BUFFER];

    index = 0;
    while ((m = chIQGetTimeout(&(_sdp)->iqueue, TIME_IMMEDIATE)) >= 0) {
        b[index] = (uint8_t)m;

        if (b[index] != VISCA_TERMINATOR) {
            index++;
        } else {
            break;
        }

        if (index == VISCA_BUFFER) {
            return -2; // overflow
        }
    }

    if (index > 0) {
        _parsePacket(b, index);
        return 0;   // packet processed
    } else {
        return -1;  // no packets available
    }
}

void ViscaController::autoReply() {
    uint8_t packet[10];
    uint8_t len = 0;

    if (_state == ViscaState_NetworkChangeReq) {
        packet[len++] = 0x88;
        packet[len++] = 0x30;
        packet[len++] = 0x01;
        packet[len++] = VISCA_TERMINATOR;

        _state = ViscaState_AddressSet;
    }

    sdWrite(_sdp, packet, len);
}

void ViscaController::_parsePacket(uint8_t* buffer, size_t length) {

    if (length == 3 && (buffer[0] & 0x0F) == 0) {

        // Network change
        if (buffer[1] == VISCA_NETWORK_CHANGE) {
            _state = ViscaState_NetworkChangeReq;
            _cam_addr = ((buffer[0] & 0xF0) >> 4) - 8; // 0xX0 -> X: 9 to F -> cam addr + 8
        }

        // ACK
        else if ((buffer[1] & 0xF0) == VISCA_RESPONSE_ACK) {
            uint8_t socket = buffer[1] & 0x0F;

            if (socket > 0 && socket <= VISCA_SOCKET_COUNT) {
                _sockets[socket - 1] = ViscaSocketState_Executing;
            }
        }

        // Complete
        else if ((buffer[1] & 0xF0) == VISCA_RESPONSE_COMPLETED) {
            uint8_t socket = buffer[1] & 0x0F;

            if (socket > 0 && socket <= VISCA_SOCKET_COUNT) {
                _sockets[socket - 1] = ViscaSocketState_Empty;
            }
        }

    } else if (length == 4) {

        // AddressSet response
        if(buffer[1] == VISCA_RESPONSE_ADDRESS && (buffer[2] & 0xF0) == 0) {
            _state = ViscaState_Active;
            _cam_addr = (buffer[2] & 0x0F) - 1; // 0x0w -> w: 2 to 8 -> cam addr + 1
        }

        // Error Message
        else if ((buffer[0] & 0x0F) == 0 && (buffer[1] & 0xF0) == VISCA_RESPONSE_ERROR) {

            if ((buffer[2] & 0x0F) == VISCA_ERROR_NO_SOCKET
                || (buffer[2] & 0x0F) == VISCA_ERROR_CMD_CANCELLED) {

                uint8_t socket = buffer[1] & 0x0F;

                if (socket > 0 && socket <= VISCA_SOCKET_COUNT) {
                    _sockets[socket - 1] = ViscaSocketState_Empty;
                }
            }
        }
    }
}

/*
 * Clear input queue buffer till next packet.
 */
void ViscaController::_clearIQ() {
    msg_t b;

    while ((b = chIQGetTimeout(&(_sdp)->iqueue, TIME_IMMEDIATE)) >= 0) {

        // 0xFF is mark for eop
        if (b == VISCA_TERMINATOR)
            return;
    }
}
