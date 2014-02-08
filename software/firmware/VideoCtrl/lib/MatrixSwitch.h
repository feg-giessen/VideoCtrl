/*
 * MatrixSwitch.h
 *
 *  Created on: 08.09.2013
 *      Author: Peter Schuster
 */

#ifndef MATRIXSWITCH_H_
#define MATRIXSWITCH_H_

#include "../net/TcpSerialAdapter.h"

class MatrixSwitch {
private:
	TcpSerialAdapter _serial;
	unsigned char _status[4];
public:
	MatrixSwitch();
	void begin(ip_addr_t addr, uint16_t port);
	void setInput(u8_t output, u8_t input);
	u8_t getInput(u8_t output);

	bool enableButtons(bool enabled);
};

#endif /* MATRIXSWITCH_H_ */
