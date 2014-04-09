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

#define DEBUG_HOST_IPADDR(p)    IP4_ADDR(p, 192, 168, 40, 11)
#define DEBUG_HOST_PORT         64781

//#define LWIP_DEBUG

#ifdef __cplusplus
extern "C" {
#endif
    void debug(uint8_t cat[4], uint8_t* data, size_t len);
    void debug_printf(uint8_t cat[4], const char* format, ...);
#ifdef __cplusplus
}
#endif

#endif /* DEBUGGER_H_ */
