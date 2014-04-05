/*
 * TcpSerialAdapter.h
 *
 *  Created on: 07.09.2013
 *      Author: Peter Schuster
 */

#include <stdint.h>
#include "lwip/ip4_addr.h"
#include "lwip/err.h"

#ifndef TCPSERIALADAPTER_H_
#define TCPSERIALADAPTER_H_

#define TCP_SERIAL_RCV_BUFFER   255

class TcpSerialAdapter {
private:
	ip_addr_t _addr;
	uint16_t _port;
public:
	TcpSerialAdapter();
	void begin(ip_addr_t addr, uint16_t port);
	err_t send(const char* data, size_t* length, char** result);
};

#endif /* TCPSERIALADAPTER_H_ */
