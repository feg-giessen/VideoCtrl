/*
 * MatrixSwitch.h
 *
 *  Created on: 08.09.2013
 *      Author: Peter Schuster
 */

#ifndef MATRIXSWITCH_H_
#define MATRIXSWITCH_H_

#include "../net/TcpSerialAdapter.h"

namespace chibios_rt {

class MatrixSwitch {
private:
	TcpSerialAdapter* _serial;
	unsigned char _status[4];
public:
	MatrixSwitch(ip_addr_t addr, uint16_t port);
	void setOutput(u8_t input, u8_t output);
	unsigned char getOutput(unsigned char input);
};

}

#endif /* MATRIXSWITCH_H_ */
