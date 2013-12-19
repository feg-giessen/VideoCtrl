/*
 * TcpSerialAdapter.cpp
 *
 *  Created on: 07.09.2013
 *      Author: Peter Schuster
 */

#include "TcpSerialAdapter.h"
#include "lwip/api.h"
#include <string.h>

TcpSerialAdapter::TcpSerialAdapter(ip_addr_t addr, uint16_t port) {
	_addr = addr;
	_port = port;
}

char* TcpSerialAdapter::send(const char* data, size_t* length) {
	struct netconn* conn;
	struct netbuf* recv_buf;
	err_t err;
	uint16_t i;
	char* p = NULL;
	char* q = NULL;

	/* Create a new TCP connection handle */
	conn = netconn_new(NETCONN_TCP);
	//conn->recv_timeout = 100; // 100 ms receive timeout;

	LWIP_ERROR("TcpSerialAdapter: invalid conn", (conn != NULL), return p;);

	err = netconn_connect(conn, &_addr, _port);
	if (err != ERR_OK)
		return p;

	i = strlen(data);
	netconn_write(conn, data, i, NETCONN_NOCOPY);

	p = NULL;
	if (!ERR_IS_FATAL(conn->last_err)) {
		err = netconn_recv(conn, &recv_buf);
		if (err == ERR_OK) {
			netbuf_data(recv_buf, (void**)&q, &i);

			// copy result to "user space"
			p = (char*)chHeapAlloc(NULL, i);
			memcpy(p, q, i);
			*length = i;
		}

		netbuf_delete(recv_buf);
	}

	netconn_close(conn);
	netconn_delete(conn);

	return p;
}
