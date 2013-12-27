/*
 * Videomischer.h
 *
 *  Created on: 25.12.2013
 *      Author: Peter Schuster
 */

#ifndef VIDEOMISCHER_H_
#define VIDEOMISCHER_H_

#include "../lib/Buttons.h"
#include "../lib/ATEM.h"
#include "../lib/SkaarhojBI8.h"

#define LED_OFF_AVAILABLE BI8_COLOR_BACKLIGHT
#define LED_OFF_UNAVAILABLE BI8_COLOR_OFF

enum ATEM_Functions {
    ATEM_None = 0,

    ATEM_Auto,
    ATEM_Cut,

    // Bus
    ATEM_Program,
    ATEM_Preview,
    ATEM_Aux1,
    ATEM_Aux2,
    ATEM_Aux3,

    // Keys
    ATEM_Dsk1,
    ATEM_Dsk2,
    ATEM_Usk1,
    ATEM_Usk2,
    ATEM_Usk3,
    ATEM_Usk4,

    // Macros
    ATEM_FadeToBlack,
    ATEM_Macro1,
    ATEM_Macro2,
    ATEM_Macro3,
    ATEM_Macro4,

    // Inputs
    ATEM_In1,
    ATEM_In2,
    ATEM_In3,
    ATEM_In4,
    ATEM_In5,
    ATEM_In6,
    ATEM_In7,
    ATEM_In8,

    // Black, BArs
    ATEM_InBlack,
    ATEM_InBars,

    // Colors
    ATEM_InColor1,
    ATEM_InColor2,

    // Mediaplayer
    ATEM_InMedia1,
    ATEM_InMedia2,

    ATEM_enum_size
};

enum ATEM_Bus {
    ATEM_Bus_Program = 0,
    ATEM_Bus_Preview,
    ATEM_Bus_Aux1,
    ATEM_Bus_Aux2,
    ATEM_Bus_Aux3,
    ATEM_Bus_enum_size
};

class Videomischer {
private:
    ATEM _atem;

    Buttons*        _buttonBoardMapping[ATEM_enum_size];
    SkaarhojBI8*    _ledBoardMapping[ATEM_enum_size];
    int             _buttonNumberMapping[ATEM_enum_size];
    int             _ledNumberMapping[ATEM_enum_size];

    ATEM_Bus _currentBus;

    bool _autoLedStatus;
    bool _ftbLedStatus;

public:
    Videomischer();

    void begin(const ip_addr_t atem_ip);

    void setButton(ATEM_Functions function, Buttons *buttons, const int number);
    void setLed(ATEM_Functions function, SkaarhojBI8 *board, const int number);

    void run();
    void doBlink();
    void deactivate();

private:
    void _setBusMode();
    void _setInputLedColors();
    void _processInputChanges();
    void _processInputToAux(uint8_t auxOutput, ATEM_Functions auxFunction, ATEM_Functions buttonDown, int inputNumber);
    void _processSpecials();
    void _tooglePictureInPicture();

    int _getAtemInputNumber(ATEM_Functions);

    bool _buttonDown(ATEM_Functions function);
    bool _buttonUp(ATEM_Functions function);
    bool _buttonIsPressed(ATEM_Functions function);
    void _buttonUpClear(ATEM_Functions function);
    void _setLed(ATEM_Functions function, int color);
};

#endif /* VIDEOMISCHER_H_ */
