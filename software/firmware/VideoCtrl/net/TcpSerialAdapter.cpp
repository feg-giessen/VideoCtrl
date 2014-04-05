/*
 * TcpSerialAdapter.cpp
 *
 *  Created on: 07.09.2013
 *      Author: Peter Schuster
 */

#include "TcpSerialAdapter.h"
#include "lwip/api.h"
#include <string.h>

TcpSerialAdapter::TcpSerialAdapter() {
}

void TcpSerialAdapter::begin(ip_addr_t addr, uint16_t port) {
	_addr = addr;
	_port = port;
}

err_t TcpSerialAdapter::send(const char* data, size_t* length, char** result) {
	struct netconn* conn;
	struct netbuf* recv_buf;
	err_t err;
	uint16_t i;
	char* p = NULL;
	char* q = NULL;

	/* Create a new TCP connection handle */
	conn = netconn_new(NETCONN_TCP);
	conn->recv_timeout = 100; // 5 ms receive timeout;

	LWIP_ERROR("TcpSerialAdapter: invalid conn", (conn != NULL), return ERR_CONN;);

	err = netconn_connect(conn, &_addr, _port);
	if (err != ERR_OK)
		return err;

	i = strlen(data);
	netconn_write(conn, data, i, NETCONN_NOCOPY);

	p = (char*)chHeapAlloc(NULL, TCP_SERIAL_RCV_BUFFER);
	i = 0;

	if (!ERR_IS_FATAL(conn->last_err)) {
	    do {
	        recv_buf = NULL;
	        err = netconn_recv(conn, &recv_buf);

	        if (err != ERR_OK) {
	            break;
	        }

		    uint16_t recv_len = 0;
			netbuf_data(recv_buf, (void**)&q, &recv_len);

			if ((i + recv_len) > TCP_SERIAL_RCV_BUFFER) {
			    break;
			}

			memcpy(p + i, q, recv_len);
			i = i + recv_len;

            if (recv_buf != NULL) {
                netbuf_delete(recv_buf);
                recv_buf = NULL;
            }
	    } while (true);
	}

    if (recv_buf != NULL) {
        netbuf_delete(recv_buf);
        recv_buf = NULL;
    }

	netconn_close(conn);
	netconn_delete(conn);

    if (i == 0) {
        chHeapFree(p);
        *result = NULL;
    } else {
        *result = p;
        err = ERR_OK;
    }

    *length = i;

	return err;
}
