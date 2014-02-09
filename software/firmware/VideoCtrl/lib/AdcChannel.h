/*
 * AdcChannel.h
 *
 *  Created on: 02.02.2014
 *      Author: Peter Schuster
 */

#ifndef ADCCHANNEL_H_
#define ADCCHANNEL_H_

#include "hal.h"

class AdcChannel {
private:
	adcsample_t* _buffer;
	uint8_t _number;
	uint8_t _count;
	uint8_t _buffer_depth;
	bool _invert;
public:
	AdcChannel();

	void begin(adcsample_t* buffer, uint8_t number, uint8_t ch_count, uint8_t buffer_depth, bool invert);

	uint16_t getValue();
};

#endif /* ADCCHANNEL_H_ */
