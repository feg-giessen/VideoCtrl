/*
 * ViscaController.cpp
 *
 *  Created on: 03.02.2014
 *      Author: Peter Schuster
 */

#include "ViscaController.h"
#include <string.h>

ViscaController::ViscaController() {
    _sdp = NULL;
    _state = ViscaState_Initial;
    _cam_addr = 1;
}

void ViscaController::begin(SerialDriver* sdp) {
    uint8_t i;

    memset(_buffer, 0, VISCA_BUFFER);
    _buffer_index = 0;

    _sdp = sdp;
    _state = ViscaState_Initial;

    for (i = 0; i < VISCA_SOCKET_COUNT; i++) {
        _sockets[i] = ViscaSocketState_Empty;
    }
}


ViscaStates ViscaController::getConnectionState() {
    return _state;
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

    while ((m = chIQGetTimeout(&(_sdp)->iqueue, TIME_IMMEDIATE)) >= 0) {
        _buffer[_buffer_index] = (uint8_t)m;

        if (_buffer[_buffer_index] != VISCA_TERMINATOR) {
            _buffer_index++;
        } else {
            _parsePacket(_buffer, _buffer_index);
            _buffer_index = 0;  // reset buffer

            return 0;   // packet processed
        }

        if (_buffer_index == VISCA_BUFFER) {
            _buffer_index = 0;  // reset buffer
            return -2; // overflow
        }
    }

    return -1;  // no packets available
}

void ViscaController::autoReply() {
    uint8_t packet[10];
    uint8_t len = 0;

    if (_state == ViscaState_NetworkChangeReq || _state == ViscaState_Initial) {
        packet[len++] = 0x88;
        packet[len++] = 0x30;
        packet[len++] = 0x01;
        packet[len++] = VISCA_TERMINATOR;

        _state = ViscaState_AddressSet;
    }

    if (len > 0) {
        sdWrite(_sdp, packet, len);
    }

}

void ViscaController::setPower(bool power) {

    uint8_t packet[10];
    uint8_t len = 0;

    packet[len++] = 0x80 | (_cam_addr & 0x0F);
    packet[len++] = 0x01;
    packet[len++] = 0x04;
    packet[len++] = 0x00;

    if (power) {
        packet[len++] = 0x02;
    } else {
        packet[len++] = 0x03;
    }

    packet[len++] = VISCA_TERMINATOR;

    sdWrite(_sdp, packet, len);
}

void ViscaController::setInfoDisplay(bool show) {

    uint8_t packet[10];
    uint8_t len = 0;

    packet[len++] = 0x80 | (_cam_addr & 0x0F);
    packet[len++] = 0x01;
    packet[len++] = 0x7E;
    packet[len++] = 0x01;
    packet[len++] = 0x18;

    if (show) {
        packet[len++] = 0x02;
    } else {
        packet[len++] = 0x03;
    }

    packet[len++] = VISCA_TERMINATOR;

    sdWrite(_sdp, packet, len);
}

void ViscaController::camMemory(uint8_t memory, bool setMem) {

    uint8_t packet[10];
    uint8_t len = 0;

    packet[len++] = 0x80 | (_cam_addr & 0x0F);
    packet[len++] = 0x01;
    packet[len++] = 0x04;
    packet[len++] = 0x3F;
	if (setMem) {
		packet[len++] = 0x01;
	} else {
		packet[len++] = 0x02;
	}
	packet[len++] = 0x00 | (memory & 0x0F);
	packet[len++] = VISCA_TERMINATOR;

	sdWrite(_sdp, packet, len);
}

void ViscaController::camDrive(uint8_t x, uint8_t y, uint8_t d1, uint8_t d2) {

	uint8_t packet[10];
	uint8_t len = 0;

	packet[len++] = 0x80 | (_cam_addr & 0x0F);
	packet[len++] = 0x01;
	packet[len++] = 0x06;
	packet[len++] = 0x01;
	packet[len++] = x;
	packet[len++] = y;
	packet[len++] = d1;
	packet[len++] = d2;
	packet[len++] = VISCA_TERMINATOR;

	sdWrite(_sdp, packet, len);
}

void ViscaController::camZoom(uint8_t z) {

	uint8_t packet[10];
	uint8_t len = 0;

	packet[len++] = 0x80 | (_cam_addr & 0x0F);
	packet[len++] = 0x01;
	packet[len++] = 0x04;
	packet[len++] = 0x07;
	packet[len++] = z;
	packet[len++] = VISCA_TERMINATOR;

	sdWrite(_sdp, packet, len);
}

void ViscaController::camFocus(uint8_t z) {

	uint8_t packet[10];
	uint8_t len = 0;

	packet[len++] = 0x80 | (_cam_addr & 0x0F);
	packet[len++] = 0x01;
	packet[len++] = 0x04;
	packet[len++] = 0x08;
	packet[len++] = z;
	packet[len++] = VISCA_TERMINATOR;

	sdWrite(_sdp, packet, len);
}

void ViscaController::camMan() {

	uint8_t packet[10];
	uint8_t len = 0;

	packet[len++] = 0x80 | (_cam_addr & 0x0F);
	packet[len++] = 0x01;
	packet[len++] = 0x04;
	packet[len++] = 0x38;
	packet[len++] = 0x03;
	packet[len++] = VISCA_TERMINATOR;

	sdWrite(_sdp, packet, len);
}

void ViscaController::camAuto() {

	uint8_t packet[10];
	uint8_t len = 0;

	packet[len++] = 0x80 | (_cam_addr & 0x0F);
	packet[len++] = 0x01;
	packet[len++] = 0x04;
	packet[len++] = 0x38;
	packet[len++] = 0x02;
	packet[len++] = VISCA_TERMINATOR;

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
