/*
 * Encoder.h
 *
 *  Created on: 18.11.2013
 *      Author: Peter Schuster
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include <stdint.h>

class Encoder {

    // Dekodertabelle für wackeligen Rastpunkt
    // halbe Auflösung
    static const int8_t _table[];

    // Dekodertabelle für normale Drehgeber
    // volle Auflösung
    //const int8_t table[16] PROGMEM = {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0};

private:
    int8_t      _value;
    int8_t      _last; // alten Wert speichern

public:
    Encoder();

    void updateValue(uint16_t data, uint8_t bit_a, uint8_t bit_b);

    int8_t getValue(bool reset);
};

#endif /* ENCODER_H_ */
