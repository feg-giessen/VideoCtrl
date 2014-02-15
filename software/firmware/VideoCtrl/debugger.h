/*
 * debugger.h
 *
 *  Created on: 15.02.2014
 *      Author: Peter Schuster
 */

#ifndef DEBUGGER_H_
#define DEBUGGER_H_

#include <stdint.h>
#include <stddef.h>

#define DEBUG 1

#define DEBUG_UDP 1

#define DEBUG_HOST_IPADDR(p)	IP4_ADDR(p, 192, 168, 40, 11)
#define DEBUG_HOST_PORT			64781

#define debug_printf(cat, ...) {		\
	char mesg[255];						\
	sprintf(mesg, __VA_ARGS__);			\
	debug((uint8_t*)cat, (uint8_t*)mesg, (size_t)strlen(mesg)); }


#ifdef __cplusplus
extern "C" {
#endif
	void debug(uint8_t cat[4], uint8_t* data, size_t len);
#ifdef __cplusplus
}
#endif

#endif /* DEBUGGER_H_ */
