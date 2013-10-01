/*
 * UdpClient.cpp
 *
 *  Created on: 14.09.2013
 *      Author: Peter Schuster
 */

#include "UdpClient.h"
#include "ch.hpp"
#include "chschd.h"
#include <string.h>
#include "lwip/ip4_addr.h"
#include "lib/Stream.h"

UdpClient::UdpClient() {
	_mbox = new chibios_rt::MailboxBuffer<16>();
	_initialized = false;
	_buf = NULL;
	_pcb = NULL;
}

void UdpClient::stop() {
	if (_initialized) {
		_mbox->reset();

		udp_disconnect(_pcb);
		udp_remove(_pcb);
		_pcb = NULL;
	}
}

void UdpClient::begin(u16_t local_port, ip_addr_t* ipaddr, u16_t port) {

	// clean-up previous connection.
	stop();

	_pcb = udp_new();

	if (local_port > 0) {
		udp_bind(_pcb, IP_ADDR_ANY, local_port);
	}

	udp_connect(_pcb, ipaddr, port);

	// set receive delegate
	udp_recv(_pcb, &UdpClient::_recv, this);

	_initialized = true;
}

void UdpClient::begin(ip_addr_t* ipaddr, u16_t port) {
	begin(0, ipaddr, port);
}

void UdpClient::send(u8_t* data, size_t len) {

	// create/append buffer
	write(data, len);

	udp_send(_pcb, _buf);

	pbuf_free(_buf);
	_buf = NULL;
}

void UdpClient::write(u8_t* data, size_t len) {

	if (_buf == NULL) {
		_buf = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_REF);
		memcpy(_buf->payload, data, len);
		_buf->len = _buf->tot_len = len;

	} else {
		struct pbuf* p = pbuf_alloc(PBUF_RAW, len, PBUF_REF);
		if (p == NULL) {
			return;
		}
		memcpy(p->payload, data, len);
		p->len = p->tot_len = len;

		struct pbuf* q;
		for (q = _buf; q->next != NULL; q = q->next) {
			/* add total length of second chain to all totals of first chain */
			q->tot_len += p->tot_len;
		}
		q->next = p;
	}
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void UdpClient::_recv (void *arg, struct udp_pcb *pcb, struct pbuf *p, ip_addr_t *addr, u16_t port) {
	((UdpClient*)arg)->_mbox->post((msg_t)p, TIME_INFINITE);
}
#pragma GCC diagnostic pop

Stream* UdpClient::read() {
	msg_t msg;
	struct pbuf* p;
	size_t len;
	uint8_t* data;
	Stream* res;

	// Fetch with timeout
	msg_t s = _mbox->fetchI(&msg);

	if (s != RDY_OK)
		return NULL;

	p = (pbuf*)msg;

	// allocate mem for payload.
	data = (u8_t*)chHeapAlloc(NULL, p->tot_len);

	// copy data to app domain
	pbuf_copy_partial(p, (void*)data, p->tot_len, 0);

	len = p->tot_len;

	// free pbuf chain
	pbuf_free(p);

	res = new Stream();
	res->load(data, len);

	return res;
}

UdpClient::~UdpClient() {
	stop();
}
