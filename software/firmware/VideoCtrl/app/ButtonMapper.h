/*
 * ButtonMapper.h
 *
 *  Created on: 27.12.2013
 *      Author: Peter Schuster
 */

#ifndef BUTTONMAPPER_H_
#define BUTTONMAPPER_H_

#include "Videomischer.h"
#include "Memory.h"

class ButtonMapper {
private:
    button_mapping_t _mapping;

    Memory* _memory;

    SkaarhojBI8* _board1;
    SkaarhojBI8* _board2;

public:
    ButtonMapper();

    void begin(Memory* memory, SkaarhojBI8* board1, SkaarhojBI8* board2);

    ATEM_Functions getFunction(uint8_t number);
    uint8_t getButtonNumber(ATEM_Functions function);

    void load(uint8_t position);
    void store(uint8_t position);

    void apply(Videomischer* mischer);
};

#endif /* BUTTONMAPPER_H_ */
