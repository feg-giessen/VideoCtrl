/*
 * PtzCamera.cpp
 *
 *  Created on: 02.02.2014
 *      Author: Peter Schuster
 */

#include <string.h>
#include "PtzCamera.h"

PtzCamera::PtzCamera() {
	uint8_t i, l;
	_x = _y = _z = NULL;

    // Initialize mapping
    l = (uint8_t)PTZ_enum_size;
    for (i = 0; i < l; i++) {
        _buttonBoardMapping[(PTZ_Functions)i]  = NULL;
        _buttonNumberMapping[(PTZ_Functions)i] = -1;
        _ledBoardMapping[(PTZ_Functions)i]     = NULL;
        _ledNumberMapping[(PTZ_Functions)i]    = -1;
    }

    // PTZ_None is not actually a function, but we ignore this fact for simplicity of the program.
}

void PtzCamera::begin(SerialDriver* sdp, AdcChannel* x, AdcChannel* y, AdcChannel* z) {
	_visca.begin(sdp);
	_x = x;
	_y = y;
	_z = z;
}

bool PtzCamera::online() {
	return false;
}

void PtzCamera::setButton(PTZ_Functions function, Buttons *buttons, const int number) {
    _buttonBoardMapping[function] = buttons;
    _buttonNumberMapping[function] = number;
}

void PtzCamera::setLed(PTZ_Functions function, SkaarhojBI8 *board, const int number) {
    _ledBoardMapping[function] = board;
    _ledNumberMapping[function] = number;
}

void PtzCamera::run() {

	// process incoming packets
	_visca.processPackets();

	// send auto replies (internal packets)
	_visca.autoReply();
}
