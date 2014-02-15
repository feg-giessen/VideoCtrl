/*
 * debugger.c
 *
 *  Created on: 15.02.2014
 *      Author: Peter Schuster
 */

#include "debugger.h"

#include <string.h>
#include "lwip/udp.h"

#if DEBUG

#if DEBUG_UDP

struct udp_pcb* debug_conn = NULL;

void debug(uint8_t cat[4], uint8_t* data, size_t len) {
	struct pbuf* buf_head;
	struct pbuf* buf_data;

	// create/append buffer
	buf_head = pbuf_alloc(PBUF_TRANSPORT, 4, PBUF_REF);
	if (buf_head == NULL)
		return;
	buf_head->payload = cat;

	buf_data = pbuf_alloc(PBUF_RAW, len, PBUF_REF);
	if (buf_data == NULL)
		return;
	buf_data->payload = data;

	pbuf_cat(buf_head, buf_data);

	if (debug_conn == NULL) {
		debug_conn = udp_new();
		ip_addr_t ip;
		DEBUG_HOST_IPADDR(&ip);
		udp_connect(debug_conn, &ip, DEBUG_HOST_PORT);
	}

	udp_send(debug_conn, buf_head);

	pbuf_free(buf_head);
}

#endif // #DEBUG_UDP

#else
void debug(uint8_t cat[4], uint8_t* data, size_t len) {
}
#endif
