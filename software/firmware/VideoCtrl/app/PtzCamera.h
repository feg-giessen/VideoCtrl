/*
 * PtzCamera.h
 *
 *  Created on: 02.02.2014
 *      Author: Peter Schuster
 */

#ifndef PTZCAMERA_H_
#define PTZCAMERA_H_

#include "hal.h"
#include "../lib/Buttons.h"
#include "../lib/SkaarhojBI8.h"
#include "../lib/AdcChannel.h"
#include "../lib/ViscaController.h"

#define abs(a)  (uint8_t)(a < 0 ? (-1 * a) : a)

enum PTZ_Functions {
	PTZ_None = 0,

	PTZ_MEM_1,
	PTZ_MEM_2,
	PTZ_MEM_3,
	PTZ_MEM_4,
	PTZ_MEM_5,
	PTZ_MEM_6,

	PTZ_MEM_Store,
	PTZ_FOCUS_AUTO,

	PTZ_enum_size
};

class PtzCamera {
private:
	ViscaController _visca;
	AdcChannel* _x;
	AdcChannel* _y;
	AdcChannel* _z;
	bool _setMem;
	bool _focusAuto;
	bool _hasInitialized;
	uint8_t _xDirection;
	uint8_t _yDirection;
	uint8_t _currentZ;
	uint8_t _currentX;
	uint8_t _currentY;
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

    void power(bool power);

private:
    bool _buttonIsPressed(PTZ_Functions function);
    bool _buttonIsDown(PTZ_Functions function);
    void _setLed(PTZ_Functions function, int color);

};

#endif /* PTZCAMERA_H_ */
