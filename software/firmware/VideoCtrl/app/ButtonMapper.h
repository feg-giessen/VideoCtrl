/*
 * ButtonMapper.h
 *
 *  Created on: 27.12.2013
 *      Author: Peter Schuster
 *
 * Load, edit, apply and store mapping of buttons to ATEM_Functions.
 */

#ifndef BUTTONMAPPER_H_
#define BUTTONMAPPER_H_

#include "Videoswitcher.h"
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

    /**
     * Returns the configured function for specified button number.
     */
    ATEM_Functions getFunction(uint8_t number);

    /**
     * Sets a mapping for specified function and button number.
     */
    void setFunction(uint8_t number, ATEM_Functions function);

    /**
     * Returns the mapped button for specified function.
     */
    uint8_t getButtonNumber(ATEM_Functions function);

    /**
     * Loads a mapping from memory.
     */
    void load(uint8_t position);

    /**
     * Stores a mapping in memory.
     */
    void store(uint8_t position);

    /**
     * Applies the currently loaded mapping to the Videoswitcher.
     */
    void apply(Videoswitcher* switcher);
};

#endif /* BUTTONMAPPER_H_ */
