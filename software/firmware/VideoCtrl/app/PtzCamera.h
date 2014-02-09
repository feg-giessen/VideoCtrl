/*
 * PtzCamera.h
 *
 *  Created on: 02.02.2014
 *      Author: Peter Schuster
 */

#ifndef PTZCAMERA_H_
#define PTZCAMERA_H_

#include "hal.h"
#include "lib/Buttons.h"
#include "lib/SkaarhojBI8.h"
#include "lib/AdcChannel.h"

enum PTZ_Functions {
	PTZ_None = 0,

	PTZ_MEM_1,
	PTZ_MEM_2,
	PTZ_MEM_3,
	PTZ_MEM_4,
	PTZ_MEM_5,
	PTZ_MEM_6,

	PTZ_MEM_Store,

	PTZ_enum_size
};

class PtzCamera {
private:
	ViscaController _visca;
	AdcChannel* _x;
	AdcChannel* _y;
	AdcChannel* _z;

    Buttons*        _buttonBoardMapping[PTZ_enum_size];
    SkaarhojBI8*    _ledBoardMapping[PTZ_enum_size];
    int             _buttonNumberMapping[PTZ_enum_size];
    int             _ledNumberMapping[PTZ_enum_size];

public:
	PtzCamera();

    void begin(SerialDriver* sdp, AdcChannel* x, AdcChannel* y, AdcChannel* z);
    bool online();
    void setButton(PTZ_Functions function, Buttons *buttons, const int number);
    void setLed(PTZ_Functions function, SkaarhojBI8 *board, const int number);

    void run();
};

#endif /* PTZCAMERA_H_ */
