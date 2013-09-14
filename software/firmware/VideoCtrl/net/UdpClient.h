/*
 * UdpClient.h
 *
 *  Created on: 14.09.2013
 *      Author: Peter Schuster
 */

#ifndef UDP_H_
#define UDP_H_

#include "lwip/udp.h"
#include "ch.hpp"

class UdpClient {
private:
	struct udp_pcb* _pcb;
	struct pbuf* _buf;
	struct chibios_rt::MailboxBuffer<16>* _mbox;

	static void _recv (void *arg, struct udp_pcb *pcb, struct pbuf *p, ip_addr_t *addr, u16_t port);

public:
	UdpClient();
	virtual ~UdpClient();
	void begin(ip_addr_t* ipaddr, u16_t port);
	void begin(u16_t local_port, ip_addr_t* ipaddr, u16_t port);
	void stop();
	void send(u8_t* data, size_t len);
	void write(u8_t* data, size_t len);
	size_t read(u8_t* data);
};

#endif /* UDP_H_ */
