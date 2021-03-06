/*
 * Videoswitcher.cpp
 *
 *  Created on: 25.12.2013
 *      Author: Peter Schuster
 */

#include "Videoswitcher.h"

static ATEM_Functions inputButtons[] = {
        ATEM_InBlack,
        ATEM_In1,
        ATEM_In2,
        ATEM_In3,
        ATEM_In4,
        ATEM_In5,
        ATEM_In6,
        ATEM_In7,
        ATEM_In8,
        ATEM_InBars,
        ATEM_InColor1,
        ATEM_InColor2,
        ATEM_Program,
        ATEM_Preview
};

static int inputFunctionCount = sizeof(inputButtons) / sizeof(ATEM_Functions);

//static ATEM_Functions specialButtons[] = {
//        ATEM_Dsk1,
//        ATEM_Dsk2,
//        ATEM_Usk1,
//        ATEM_Usk2,
//        ATEM_Usk3,
//        ATEM_Usk4,
//        ATEM_Macro1,
//        ATEM_Macro2,
//        ATEM_Macro3,
//        ATEM_Macro4
//};
//
//static int specialFunctionCount = sizeof(specialButtons) / sizeof(ATEM_Functions);

Videoswitcher::Videoswitcher() {
    _faderLocked = false;
}

void Videoswitcher::begin(const ip_addr_t atem_ip, AdcChannel* fader, LedController* ledCtrl) {
    uint8_t i, l;

    _fader = fader;
    _ledCtrl = ledCtrl;

    _atem.begin(atem_ip);

    // Always start in preview
    _currentBus = ATEM_Bus_Preview;

    _autoLedStatus = false;
    _ftbLedStatus  = false;

    // Initialize mapping
    l = (uint8_t)ATEM_enum_size;
    for (i = 0; i < l; i++) {
        _buttonBoardMapping[(ATEM_Functions)i]  = NULL;
        _buttonNumberMapping[(ATEM_Functions)i] = -1;
        _ledBoardMapping[(ATEM_Functions)i]     = NULL;
        _ledNumberMapping[(ATEM_Functions)i]    = -1;
    }

    // ATEM_None is not actually a function, but we ignore this fact for simplicity of the program.
}

void Videoswitcher::connect() {
	_atem.connect();
}

bool Videoswitcher::online() {
	return !_atem.isConnectionTimedOut() && !_atem.isConnecting();
}

void Videoswitcher::setButton(ATEM_Functions function, Buttons *buttons, const int number) {
    _buttonBoardMapping[function] = buttons;
    _buttonNumberMapping[function] = number;
}

void Videoswitcher::setLed(ATEM_Functions function, SkaarhojBI8 *board, const int number) {
    _ledBoardMapping[function] = board;
    _ledNumberMapping[function] = number;
}

void Videoswitcher::run() {
    _atem.runLoop();

    if (_atem.isConnectionTimedOut()) {
        deactivate();
        if (!_atem.isConnecting()) {
        	// we lost the connection -> re-init (if !hasInitialized() were still in process of initial connect).
        	_atem.connect();
        }
    }
    else if (!_atem.isConnecting()) { // is online

        _setBusMode();

        // Set leds of input buttons
        _setInputLedColors();

        // Process pressed input buttons
        _processInputChanges();

        // Process the analog fade ctrl.
        _processFader();

        // Process changes on special ops
        _processSpecials();

        if (_buttonIsPressed(ATEM_Cut)) {
        	_setLed(ATEM_Cut, BI8_COLOR_RED);
        } else {
        	_setLed(ATEM_Cut, LED_OFF_AVAILABLE);
        }

        if (_buttonDown(ATEM_FadeToBlack)) {
            _atem.fadeToBlackActivate();
        } else if (_buttonDown(ATEM_Cut)) {
            _atem.doCut();
        } else if (_buttonDown(ATEM_Auto)) {
            _atem.doAuto();
        }
    }
}

void Videoswitcher::doBlink() {

	if (online()) {

		// FadeToBlack - blink on active (red)
		if (_atem.getFadeToBlackState() || _atem.getFadeToBlackInProgress()) {
			_setLed(ATEM_FadeToBlack, _ftbLedStatus ? LED_OFF_AVAILABLE : BI8_COLOR_RED);
			_ftbLedStatus = !_ftbLedStatus;
		} else {
			_setLed(ATEM_FadeToBlack, LED_OFF_AVAILABLE);
		}

		bool invert = _atem.getTransitionBase() == ATEM_TrBase_Bottom;

		// Auto - blink during transition (red)
		uint16_t transitionPosition = _atem.getTransitionPosition();
		if (transitionPosition > 0 && transitionPosition < 10000) {
			_setLed(ATEM_Auto, _autoLedStatus ? LED_OFF_AVAILABLE : BI8_COLOR_RED);

			if (_autoLedStatus) {
		        _ledCtrl->writeLed(LED_FADE_LOW_GREEN, 0);
		        _ledCtrl->writeLed(LED_FADE_LOW_RED, 0);
		        _ledCtrl->writeLed(LED_FADE_HIGH_GREEN, 0);
		        _ledCtrl->writeLed(LED_FADE_HIGH_RED, 0);
		    } else if ((transitionPosition >= 5000 && !invert)
                    || (transitionPosition < 5000 && invert)) {
                _ledCtrl->writeLed(LED_FADE_LOW_GREEN, 0);
                _ledCtrl->writeLed(LED_FADE_LOW_RED, 0);
                _ledCtrl->writeLed(LED_FADE_HIGH_GREEN, _faderLocked ? 100 : 0);
                _ledCtrl->writeLed(LED_FADE_HIGH_RED, _faderLocked ? 0 : 100);
            } else {
                _ledCtrl->writeLed(LED_FADE_LOW_GREEN, _faderLocked ? 100 : 0);
                _ledCtrl->writeLed(LED_FADE_LOW_RED, _faderLocked ? 0 : 100);
                _ledCtrl->writeLed(LED_FADE_HIGH_GREEN, 0);
                _ledCtrl->writeLed(LED_FADE_HIGH_RED, 0);
            }

			_autoLedStatus = !_autoLedStatus;
		} else {
			_setLed(ATEM_Auto, LED_OFF_AVAILABLE);

			// Fader LEDs are static on, if not in progress.
			if ((transitionPosition >= 5000 && !invert)
			        || (transitionPosition < 5000 && invert)) {
		        _ledCtrl->writeLed(LED_FADE_LOW_GREEN, 0);
		        _ledCtrl->writeLed(LED_FADE_LOW_RED, 0);
		        _ledCtrl->writeLed(LED_FADE_HIGH_GREEN, _faderLocked ? 100 : 0);
		        _ledCtrl->writeLed(LED_FADE_HIGH_RED, _faderLocked ? 0 : 100);
			} else {
                _ledCtrl->writeLed(LED_FADE_LOW_GREEN, _faderLocked ? 100 : 0);
                _ledCtrl->writeLed(LED_FADE_LOW_RED, _faderLocked ? 0 : 100);
                _ledCtrl->writeLed(LED_FADE_HIGH_GREEN, 0);
                _ledCtrl->writeLed(LED_FADE_HIGH_RED, 0);
			}
		}
	} else {
	    _ledCtrl->writeLed(LED_FADE_LOW_GREEN, 0);
        _ledCtrl->writeLed(LED_FADE_LOW_RED, 0);
        _ledCtrl->writeLed(LED_FADE_HIGH_GREEN, 0);
        _ledCtrl->writeLed(LED_FADE_HIGH_RED, 0);
	}
}

void Videoswitcher::deactivate() {
    uint8_t i, l;

   l = (uint8_t)ATEM_enum_size;
   for (i = 0; i < l; i++) {
       _setLed((ATEM_Functions)i, LED_OFF_UNAVAILABLE);
   }
}

uint8_t Videoswitcher::getVideoFormat() {
    return _atem.getVideoFormat();
}

void Videoswitcher::_setBusMode() {

    bool auxPressed = _buttonIsPressed(ATEM_Aux1)
            | _buttonIsPressed(ATEM_Aux2)
            | _buttonIsPressed(ATEM_Aux3);

    // Check/Set mode for input selection buttons

    // Only change to program/preview if no aux buttons is pressed.
    // If an aux button is pressed, the aux bus is set to program/preview.
    if (!auxPressed && _buttonUp(ATEM_Program)) {
        _currentBus = ATEM_Bus_Program;
    }
    else if (!auxPressed && _buttonUp(ATEM_Preview)) {
        _currentBus = ATEM_Bus_Preview;
    }
    else if (_buttonUp(ATEM_Aux1)) {
        _currentBus = ATEM_Bus_Aux1;
    }
    else if (_buttonUp(ATEM_Aux2)) {
        _currentBus = ATEM_Bus_Aux2;
    }
    else if (_buttonUp(ATEM_Aux3)) {
        _currentBus = ATEM_Bus_Aux3;
    }

    // Set leds of bank select buttons
    if (_currentBus == ATEM_Bus_Preview || _currentBus == ATEM_Bus_Program) {
    	// if in AUX mode, pgm/prv could be selected
		_setLed(ATEM_Program,   _currentBus == ATEM_Bus_Program ? BI8_COLOR_RED     : LED_OFF_AVAILABLE);
		_setLed(ATEM_Preview,   _currentBus == ATEM_Bus_Preview ? BI8_COLOR_GREEN   : LED_OFF_AVAILABLE);
    }
    _setLed(ATEM_Aux1,      _currentBus == ATEM_Bus_Aux1    ? BI8_COLOR_YELLOW  : LED_OFF_AVAILABLE);
    _setLed(ATEM_Aux3,      _currentBus == ATEM_Bus_Aux2    ? BI8_COLOR_YELLOW  : LED_OFF_AVAILABLE);
    _setLed(ATEM_Aux2,      _currentBus == ATEM_Bus_Aux3    ? BI8_COLOR_YELLOW  : LED_OFF_AVAILABLE);
}

void Videoswitcher::_processInputChanges() {
    int i, number;
    bool down;

    for (i = 0; i < inputFunctionCount; i++) {

        ATEM_Functions current = inputButtons[i];

        // Skip program and preview inputs for non-aux busses
        if ((_currentBus == ATEM_Bus_Program && current == ATEM_Program)
                || (_currentBus == ATEM_Bus_Preview && current == ATEM_Preview))
            continue;

        number = _getAtemInputNumber(current);
        down = _buttonDown(current);

        if (number < 0 || !down)
            continue;

        if (_currentBus == ATEM_Bus_Program) {
            _atem.changeProgramInput(number);
        } else if (_currentBus == ATEM_Bus_Preview) {
            _atem.changePreviewInput(number);
        } else {
            if (_currentBus == ATEM_Bus_Aux1) {
                _processInputToAux(1, ATEM_Aux1, current, number);
            } else if (_currentBus == ATEM_Bus_Aux2) {
                _processInputToAux(2, ATEM_Aux2, current, number);
            } else if (_currentBus == ATEM_Bus_Aux3) {
                _processInputToAux(3, ATEM_Aux3, current, number);
            }
        }
    }
}

void Videoswitcher::_processInputToAux(uint8_t auxOutput, ATEM_Functions auxFunction, ATEM_Functions buttonDown, int inputNumber) {
    bool auxPressed;

    auxPressed = false;

    if (buttonDown == ATEM_Program || buttonDown == ATEM_Preview) {
        auxPressed = _buttonIsPressed(auxFunction);

        // Change aux to program/preview, only if respective aux button is pressed
        if (auxPressed) {
            _atem.changeAuxState(auxOutput, inputNumber);

            // Don't handle release ("up" action) of this aux button
            _buttonUpClear(auxFunction);
        }
    } else {
        _atem.changeAuxState(auxOutput, inputNumber);
    }
}

void Videoswitcher::_processSpecials() {
    bool program, preview;

    program = _currentBus == ATEM_Bus_Program;
    preview = _currentBus == ATEM_Bus_Preview;

    if (program) {

        if (_buttonDown(ATEM_Dsk1)) {
            _atem.changeDownstreamKeyOn(1, !_atem.getDownstreamKeyerStatus(1));
        }
        if (_buttonDown(ATEM_Dsk2)) {
            _atem.changeDownstreamKeyOn(2, !_atem.getDownstreamKeyerStatus(2));
        }

        if (_buttonDown(ATEM_Usk1)) {
            _atem.changeUpstreamKeyOn(1, !_atem.getUpstreamKeyerStatus(1));
        }
        if (_buttonDown(ATEM_Usk2)) {
            _atem.changeUpstreamKeyOn(2, !_atem.getUpstreamKeyerStatus(2));
        }
        if (_buttonDown(ATEM_Usk3)) {
            _atem.changeUpstreamKeyOn(3, !_atem.getUpstreamKeyerStatus(3));
        }
        if (_buttonDown(ATEM_Usk4)) {
            _atem.changeUpstreamKeyOn(4, !_atem.getUpstreamKeyerStatus(4));
        }

        if (_buttonDown(ATEM_Macro1)) {
            _tooglePictureInPicture();
        }

        _setLed(ATEM_Dsk1, _atem.getDownstreamKeyerStatus(1) ? BI8_COLOR_YELLOW : LED_OFF_AVAILABLE);
        _setLed(ATEM_Dsk2, _atem.getDownstreamKeyerStatus(2) ? BI8_COLOR_YELLOW : LED_OFF_AVAILABLE);
        _setLed(ATEM_Usk1, _atem.getUpstreamKeyerStatus(1) ? BI8_COLOR_YELLOW : LED_OFF_AVAILABLE);
        _setLed(ATEM_Usk2, _atem.getUpstreamKeyerStatus(2) ? BI8_COLOR_YELLOW : LED_OFF_AVAILABLE);
        _setLed(ATEM_Usk3, _atem.getUpstreamKeyerStatus(3) ? BI8_COLOR_YELLOW : LED_OFF_AVAILABLE);
        _setLed(ATEM_Usk4, _atem.getUpstreamKeyerStatus(4) ? BI8_COLOR_YELLOW : LED_OFF_AVAILABLE);

        _setLed(ATEM_Macro1, _atem.getUpstreamKeyerStatus(1) ? BI8_COLOR_YELLOW : LED_OFF_AVAILABLE);

    } else if (preview) {

        if (_buttonDown(ATEM_Dsk1)) {
            _atem.changeDownstreamKeyTie(1, !_atem.getDownstreamKeyTie(1));
        }
        if (_buttonDown(ATEM_Dsk2)) {
            _atem.changeDownstreamKeyTie(2, !_atem.getDownstreamKeyTie(2));
        }

        if (_buttonDown(ATEM_Usk1)) {
            _atem.changeUpstreamKeyNextTransition(1, !_atem.getUpstreamKeyerOnNextTransitionStatus(1));
        }
        if (_buttonDown(ATEM_Usk2)) {
            _atem.changeUpstreamKeyNextTransition(2, !_atem.getUpstreamKeyerOnNextTransitionStatus(2));
        }
        if (_buttonDown(ATEM_Usk3)) {
            _atem.changeUpstreamKeyNextTransition(3, !_atem.getUpstreamKeyerOnNextTransitionStatus(3));
        }
        if (_buttonDown(ATEM_Usk4)) {
            _atem.changeUpstreamKeyNextTransition(4, !_atem.getUpstreamKeyerOnNextTransitionStatus(4));
        }

        _setLed(ATEM_Dsk1, _atem.getDownstreamKeyTie(1) ? BI8_COLOR_YELLOW : LED_OFF_AVAILABLE);
        _setLed(ATEM_Dsk2, _atem.getDownstreamKeyTie(2) ? BI8_COLOR_YELLOW : LED_OFF_AVAILABLE);
        _setLed(ATEM_Usk1, _atem.getUpstreamKeyerOnNextTransitionStatus(1) ? BI8_COLOR_YELLOW : LED_OFF_AVAILABLE);
        _setLed(ATEM_Usk2, _atem.getUpstreamKeyerOnNextTransitionStatus(2) ? BI8_COLOR_YELLOW : LED_OFF_AVAILABLE);
        _setLed(ATEM_Usk3, _atem.getUpstreamKeyerOnNextTransitionStatus(3) ? BI8_COLOR_YELLOW : LED_OFF_AVAILABLE);
        _setLed(ATEM_Usk4, _atem.getUpstreamKeyerOnNextTransitionStatus(4) ? BI8_COLOR_YELLOW : LED_OFF_AVAILABLE);

        _setLed(ATEM_Macro1, LED_OFF_UNAVAILABLE);
    } else {
        // Keys not available for aux bus

        _setLed(ATEM_Dsk1, LED_OFF_UNAVAILABLE);
        _setLed(ATEM_Dsk2, LED_OFF_UNAVAILABLE);
        _setLed(ATEM_Usk1, LED_OFF_UNAVAILABLE);
        _setLed(ATEM_Usk2, LED_OFF_UNAVAILABLE);
        _setLed(ATEM_Usk3, LED_OFF_UNAVAILABLE);
        _setLed(ATEM_Usk4, LED_OFF_UNAVAILABLE);
        _setLed(ATEM_Macro1, LED_OFF_UNAVAILABLE);
    }
}

void Videoswitcher::_processFader() {
    // 0-1000
    uint16_t position = _atem.getTransitionPosition();
    if (position > 10000) {
        position = 10000;
    }

    bool invert = _atem.getTransitionBase() == ATEM_TrBase_Bottom;

    // 60 mm -> 4096 values -> 68 values per mm --> about 6 bits per mm
    uint16_t fader_position = _fader->getValue();
    if (fader_position > 4096) {
        fader_position = 4096;
    }
    if (invert) {
        fader_position = 4096 - fader_position;
    }

    // 10,000 = 4096 *
    if (fader_position < 48) {
        fader_position = 0;
    } else {
        fader_position = fader_position - 48;
    }
    // fader_position = fader_position * 2 + fader_position / 2; => x 2.5
    fader_position = (fader_position << 1) + (fader_position >> 2);

    if (fader_position > 10000) {
        fader_position = 10000;
    }

    int16_t diff = (int16_t)fader_position - position;
    diff = diff & ~0x3; // clear last two bits

    if (_faderLocked && diff != 0) {
        // fader is (was) locked -> change ATEM fade position
        //_atem.changeTransitionPosition(fader_position);
        if (fader_position == 10000) {
            // we're done -> send "0" to complete transition.
            //_atem.changeTransitionPosition(0);
        }
    } else {
        _faderLocked = (diff == 0);
    }
}

void Videoswitcher::_tooglePictureInPicture() {

    // based on Kaspar Skaarhojs pip solution
    _atem.changeDVESettingsTemp(8500,-4500,300,300);

    // First, store original preview input:
    uint8_t tempPreviewInput = _atem.getPreviewInput();

    // Then, set preview=program (so auto doesn't change input)
    _atem.changePreviewInput(_atem.getProgramInput());
    while(_atem.getProgramInput() != _atem.getPreviewInput()) {

        // process packets
        _atem.runLoop();
    }

    // Then set transition status:
    bool tempOnNextTransitionStatus = _atem.getUpstreamKeyerOnNextTransitionStatus(1);
    _atem.changeUpstreamKeyNextTransition(1, true); // Set upstream key next transition
    while(!_atem.getUpstreamKeyerOnNextTransitionStatus(1)) {

        // process packets
        _atem.runLoop();
    }

    // Make Auto Transition:
    _atem.doAuto();
    while(_atem.getTransitionPosition() == 0) {

        // process packets
        _atem.runLoop();
    }
    while(_atem.getTransitionPosition() > 0) {

        // process packets
        _atem.runLoop();
    }

    // Then reset transition status:
    _atem.changeUpstreamKeyNextTransition(1, tempOnNextTransitionStatus);
    while(tempOnNextTransitionStatus != _atem.getUpstreamKeyerOnNextTransitionStatus(1)) {

        // process packets
        _atem.runLoop();
    }

    // Reset preview bus:
    _atem.changePreviewInput(tempPreviewInput);
    while(tempPreviewInput != _atem.getPreviewInput()) {

        // process packets
        _atem.runLoop();
    }
}

void Videoswitcher::_setInputLedColors() {
    uint8_t program, preview, aux1, aux2, aux3;

    program = _atem.getProgramInput();
    preview = _atem.getPreviewInput();
    aux1 = _atem.getAuxState(1);
    aux2 = _atem.getAuxState(2);
    aux3 = _atem.getAuxState(3);

    int i, color, number;
    for (i = 0; i < inputFunctionCount; i++) {
        ATEM_Functions current = inputButtons[i];

        number = _getAtemInputNumber(current);

        if (number < 0)
            continue;

        // Default is "OFF"
        color = LED_OFF_AVAILABLE;

        if (_currentBus == ATEM_Bus_Program || _currentBus == ATEM_Bus_Preview) {

            // Skip program and preview inputs
            if (current == ATEM_Program || current == ATEM_Preview)
                continue;

            // Red/green buttons
            if (((uint8_t)number) == program) {
                color = BI8_COLOR_RED;
            } else if (((uint8_t)number) == preview) {
                color = BI8_COLOR_GREEN;
            }
        } else {
            // aux 1-3 -> Yellow buttons
            if (_currentBus == ATEM_Bus_Aux1 && ((uint8_t)number) == aux1) {
                color = BI8_COLOR_YELLOW;
            } else if (_currentBus == ATEM_Bus_Aux2 && ((uint8_t)number) == aux2) {
                color = BI8_COLOR_YELLOW;
            } else if (_currentBus == ATEM_Bus_Aux3 && ((uint8_t)number) == aux3) {
                color = BI8_COLOR_YELLOW;
            }
        }

        _setLed(current, color);
    }
}

int Videoswitcher::_getAtemInputNumber(ATEM_Functions function) {
    /* On ATEM 1M/E:
     * Black (0),
     * 1 (1), 2 (2), 3 (3), 4 (4), 5 (5), 6 (6), 7 (7), 8 (8),
     * Bars (9),
     * Color1 (10), Color 2 (11),
     * Media 1 (12), Media 1 Key (13),
     * Media 2 (14), Media 2 Key (15),
     * Program (16), Preview (17),
     * Clean1 (18), Clean 2 (19)
     */
    bool ver42 = _atem.ver42();

    switch (function) {
    case ATEM_InBlack:  return 0;
    case ATEM_In1:      return 1;
    case ATEM_In2:      return 2;
    case ATEM_In3:      return 3;
    case ATEM_In4:      return 4;
    case ATEM_In5:      return 5;
    case ATEM_In6:      return 6;
    case ATEM_In7:      return 7;
    case ATEM_In8:      return 8;
    case ATEM_InBars:   return ver42 ?  1000 :  9;
    case ATEM_InColor1: return ver42 ?  2001 : 10;
    case ATEM_InColor2: return ver42 ?  2002 : 11;
    case ATEM_InMedia1: return ver42 ?  3010 : 12;
    case ATEM_InMedia2: return ver42 ?  3020 : 14;
    case ATEM_Program:  return ver42 ? 10010 : 16;
    case ATEM_Preview:  return ver42 ? 10011 : 17;
    default:
        return -1;
    }
}

bool Videoswitcher::_buttonDown(ATEM_Functions function) {
    Buttons* board;
    int number;

    board = _buttonBoardMapping[function];

    if (board == NULL)
        return false;

    number = _buttonNumberMapping[function];
    return board->buttonDown(number);
}

bool Videoswitcher::_buttonUp(ATEM_Functions function) {
    Buttons* board;
    int number;

    board = _buttonBoardMapping[function];

    if (board == NULL)
        return false;

    number = _buttonNumberMapping[function];
    return board->buttonUp(number);
}

bool Videoswitcher::_buttonIsPressed(ATEM_Functions function) {
    Buttons* board;
    int number;

    board = _buttonBoardMapping[function];

    if (board == NULL)
        return false;

    number = _buttonNumberMapping[function];
    return board->buttonIsPressed(number);
}

void Videoswitcher::_buttonUpClear(ATEM_Functions function) {
    Buttons* board;
    int number;

    board = _buttonBoardMapping[function];

    if (board == NULL)
        return;

    number = _buttonNumberMapping[function];
    board->handleUp(number);
}

void Videoswitcher::_setLed(ATEM_Functions function, int color) {
    SkaarhojBI8* board;
    int number;

    board = _ledBoardMapping[function];

    if (board == NULL)
        return;

    number = _ledNumberMapping[function];
    return board->setButtonColor(number, color);
}
