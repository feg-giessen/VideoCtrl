/*
 * EthernetUDP.cpp
 *
 *  Created on: 10.09.2013
 *      Author: Peter Schuster
 */

#include "EthernetUDP.h"
#include "lwip/ip4_addr.h"

EthernetUDP::EthernetUDP() {
}

void EthernetUDP::begin(u16_t local_port, ip_addr_t ipaddr, u16_t port) {
	struct netconn* conn;
	conn = netconn_new(NETCONN_UDP);

	netconn_bind(conn, IP_ADDR_ANY, local_port);
	netconn_connect(conn, &ipaddr, port);

	netbuf* buf;
	netconn_recv(conn, &buf);

	u8_t data[96];
	u16_t data_len;
	netbuf_data(buf, &data, &data_len);

	netbuf_delete(buf);
}

void EthernetUDP::send(u8_t* data, size_t len) {
	netconn_connect(conn, &ipaddr, port);

	write(data, len);

	netconn_send(conn, _send_buf);

	netbuf_delete(_send_buf);
	_send_buf = NULL;

	netconn_disconnect(conn);
}

void EthernetUDP::write(u8_t* data, size_t len) {

	if (_send_buf == NULL) {
		netbuf* buf = netbuf_new();
		netbuf_ref(buf, data, len);

		_send_buf = buf;
	} else {
		struct pbuf* p = pbuf_alloc(PBUF_RAW, 0, PBUF_REF);
		if (p == NULL) {
			return ERR_MEM;
		}
		p->payload = (void*)data;
		p->len = len;

		struct pbuf* q;
		for (q = _send_buf->p; q->next != NULL; q = q->next) {
			/* add total length of second chain to all totals of first chain */
			q->tot_len += p->tot_len;
		}
		q->next = p;
	}
}
