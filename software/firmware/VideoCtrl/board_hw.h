/*
 * board_hw.h
 *
 *  Created on: 28.10.2013
 *      Author: Peter Schuster
 */

#ifndef _BOARD_HW_H_
#define _BOARD_HW_H_

#include "hal.h"
#include "halconf.h"

// VISCA to use UART (-> TRUE) or SERIAL (-> FALSE) driver?
#define VISCA_UART STM32_UART_USE_USART2

#ifdef __cplusplus
extern "C" {
#endif

void init_board_hal(void);

#ifdef __cplusplus
}
#endif

#endif // _BOARD_HW_H_
