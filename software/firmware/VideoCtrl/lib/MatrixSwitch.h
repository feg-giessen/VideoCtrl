/*
 * MatrixSwitch.h
 *
 *  Created on: 08.09.2013
 *      Author: Peter Schuster
 */

#ifndef MATRIXSWITCH_H_
#define MATRIXSWITCH_H_

#include "ch.hpp"
#include "RemoteAdapter.h"

typedef struct {
    u8_t input;
    u8_t output;
} matrix_msg_t;

class MatrixSwitch : public RemoteAdapter {
private:
	unsigned char _status[4];
public:
	MatrixSwitch();
	void begin(ip_addr_t addr, uint16_t port);
	void setInput(u8_t output, u8_t input);
	u8_t getInput(u8_t output);

	bool enableButtons(bool enabled);

private:
	void static _recv_cb(err_t err, void* context, char* result, size_t length, void* arg);
};

#endif /* MATRIXSWITCH_H_ */
