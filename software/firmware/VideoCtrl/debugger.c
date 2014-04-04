/*
 * debugger.c
 *
 *  Created on: 15.02.2014
 *      Author: Peter Schuster
 */

#include "debugger.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "lwip/udp.h"

#if DEBUG

#if DEBUG_UDP

struct udp_pcb* debug_conn = NULL;
char debug_f_mesg[255];
char debug_f_mesg_init = 0;

void debug_printf(uint8_t cat[4], const char* format, ...) {

	if (debug_f_mesg_init == 0) {
		memset(debug_f_mesg, 0, 255);
		debug_f_mesg_init = 1;
	}

	va_list argptr;
	va_start(argptr, format);

	vsprintf(debug_f_mesg, format, argptr);

	va_end(argptr);

	debug(cat, (uint8_t*)debug_f_mesg, (size_t)strlen(debug_f_mesg));
}

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

	pbuf_chain(buf_head, buf_data);
	if (buf_data->ref > 1) {
		// decrease ref counter (we're not keeping a reference to this...)
		buf_data->ref -= 1;
	}

	if (debug_conn == NULL) {
		debug_conn = udp_new();
		ip_addr_t ip;
		DEBUG_HOST_IPADDR(&ip);
		udp_connect(debug_conn, &ip, DEBUG_HOST_PORT);
	}

	udp_send(debug_conn, buf_head);

	if (pbuf_free(buf_head) < 2) {
		pbuf_free(buf_data);
	}
}

#endif // #DEBUG_UDP

#else
void debug(uint8_t cat[4], uint8_t* data, size_t len) {
}
void debug_printf(uint8_t cat[4], const char* format, ...) {
}
#endif
