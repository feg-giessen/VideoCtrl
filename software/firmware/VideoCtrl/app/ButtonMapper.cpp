/*
 * ButtonMapper.cpp
 *
 *  Created on: 27.12.2013
 *      Author: Peter Schuster
 */

#include "ButtonMapper.h"

ButtonMapper::ButtonMapper() {
}

void ButtonMapper::begin(Memory* memory, SkaarhojBI8* board1, SkaarhojBI8* board2) {
    _memory = memory;
    _board1 = board1;
    _board2 = board2;
}

ATEM_Functions ButtonMapper::getFunction(uint8_t number) {
    if (number < 1 || number > 16)
        return ATEM_None;

    uint8_t* p;
    p = &(_mapping.b1);
    p += (number - 1);

    return (ATEM_Functions)*p;
}

void ButtonMapper::setFunction(uint8_t number, ATEM_Functions function) {
    if (number < 1 || number > 16)
        return;

    uint8_t* p;
    p = &(_mapping.b1);
    p += (number - 1);

    *p = (uint8_t)function;
}

uint8_t ButtonMapper::getButtonNumber(ATEM_Functions function) {
    if (function == ATEM_None || function == ATEM_enum_size)
        return 0;

    uint8_t i;
    uint8_t* p;
    p = &(_mapping.b1);

    for (i = 1; i <= 16; i++) {
        if (*p == (uint8_t)function) {
            return i;
        }

        p = p + 1;
    }

    return 0;
}

void ButtonMapper::load(uint8_t position) {
    _memory->getButtonMapping(position, &_mapping);
}

void ButtonMapper::store(uint8_t position) {
    _memory->setButtonMapping(position, &_mapping);
}

void ButtonMapper::apply(Videoswitcher* switcher) {
    if (switcher == NULL)
        return;

    uint8_t i;
    uint8_t* p;

    // Buttons 1-8 of board 1
    p = &(_mapping.b1);
    for (i = 1; i <= 8; i++) {

        switcher->setButton( (ATEM_Functions)*p, _board1, i);
        switcher->setLed(    (ATEM_Functions)*p, _board1, i);

        p++;
    }

    // Buttons 1-8 of board 2
    p = &(_mapping.b9);
    for (i = 1; i <= 8; i++) {

        switcher->setButton( (ATEM_Functions)*p, _board2, i);
        switcher->setLed(    (ATEM_Functions)*p, _board2, i);

        p++;
    }
}
