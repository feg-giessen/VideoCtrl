/*
 * TcpSerialAdapter.h
 *
 *  Created on: 07.09.2013
 *      Author: Peter Schuster
 */

#include <stdint.h>
#include "lwip/ip4_addr.h"

#ifndef TCPSERIALADAPTER_H_
#define TCPSERIALADAPTER_H_

class TcpSerialAdapter {
public:
	TcpSerialAdapter(ip_addr_t addr, uint16_t port);
	char* send(const char* data, size_t* length);
private:
	ip_addr_t _addr;
	uint16_t _port;
};

#endif /* TCPSERIALADAPTER_H_ */
