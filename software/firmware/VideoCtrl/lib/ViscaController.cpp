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

    while ((msg = getPacket()) != VISCA_PROC_NO_PCKTS) {

        // handle overflow
        if (msg == VISCA_PROC_OVERFLOW) {
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
            return VISCA_PROC_OVERFLOW;
        }
    }

    return VISCA_PROC_NO_PCKTS;
}

void ViscaController::autoReply() {
    uint8_t packet[10];
    uint8_t len = 0;

    if (_state == ViscaState_NetworkChangeReq || _state == ViscaState_Initial) {
        packet[len++] = 0x88;
        packet[len++] = VISCA_RESPONSE_ADDRESS;
        packet[len++] = 0x01;
        packet[len++] = VISCA_TERMINATOR;

        _state = ViscaState_AddressSet;
    }

    if (len > 0) {
        sdWrite(_sdp, packet, len);
    }

}

void ViscaController::setPower(bool power) {

    uint8_t packet[] = {
        VISCA_HEADER(_cam_addr),
        VISCA_COMMAND,
        VISCA_CATEGORY_CAMERA1,
        VISCA_POWER,
        (uint8_t)(power ? VISCA_ON : VISCA_OFF),
        VISCA_TERMINATOR
    };

    sdWrite(_sdp, packet, 6);
}

void ViscaController::setInfoDisplay(bool show) {

    uint8_t packet[] = {
        VISCA_HEADER(_cam_addr),
        VISCA_COMMAND,
        VISCA_INFO_DISPLAY,
        0x01,
        0x18,
        (uint8_t)(show ? VISCA_ON : VISCA_OFF),
        VISCA_TERMINATOR
    };

    sdWrite(_sdp, packet, 7);
}

void ViscaController::camMemory(uint8_t memory, bool setMem) {

    uint8_t packet[] = {
        VISCA_HEADER(_cam_addr),
        VISCA_COMMAND,
        VISCA_CATEGORY_CAMERA1,
        VISCA_MEMORY,
        (uint8_t)(setMem ? VISCA_MEMORY_SET : VISCA_MEMORY_RECALL),
        (uint8_t)(memory & 0x0F),
	    VISCA_TERMINATOR
    };

	sdWrite(_sdp, packet, 7);
}

void ViscaController::camDrive(uint8_t x, uint8_t y, uint8_t d1, uint8_t d2) {

	uint8_t packet[] = {
        VISCA_HEADER(_cam_addr),
        VISCA_COMMAND,
        VISCA_CATEGORY_PAN_TILTER,
        VISCA_PT_DRIVE,
        x,
        y,
        d1,
        d2,
        VISCA_TERMINATOR
	};

	sdWrite(_sdp, packet, 9);
}

void ViscaController::camZoom(uint8_t z) {

    uint8_t packet[] = {
        VISCA_HEADER(_cam_addr),
        VISCA_COMMAND,
        VISCA_CATEGORY_CAMERA1,
        VISCA_ZOOM,
        z,
        VISCA_TERMINATOR
    };

	sdWrite(_sdp, packet, 6);
}

void ViscaController::camFocus(uint8_t z) {

    uint8_t packet[] = {
        VISCA_HEADER(_cam_addr),
        VISCA_COMMAND,
	    VISCA_CATEGORY_CAMERA1,
	    VISCA_FOCUS,
	    z,
	    VISCA_TERMINATOR
    };

	sdWrite(_sdp, packet, 6);
}

void ViscaController::camAutoFocus(bool value) {

    uint8_t packet[] = {
        VISCA_HEADER(_cam_addr),
        VISCA_COMMAND,
	    VISCA_CATEGORY_CAMERA1,
	    VISCA_FOCUS_AUTO,
	    (uint8_t)(value ? VISCA_ON : VISCA_OFF),
	    VISCA_TERMINATOR
    };

	sdWrite(_sdp, packet, 6);
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
